#include "CameraClass.h"

CameraClass::CameraClass(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(1.0f);
	this->defaultMovementRate = 10.0f;
	this->defaultMouseSensitivity = 100.0f;

	this->initiateMatrices();
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
}

CameraClass::~CameraClass()
{

}

DirectX::XMMATRIX CameraClass::viewProjectionMatrix()
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&this->mViewMatrix);
	DirectX::XMMATRIX projectionMatrix = DirectX::XMLoadFloat4x4(&this->mProjectionMatrix);
	
	return DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);
}

void CameraClass::setPosition(float x, float y, float z)
{
	this->mPosition = DirectX::XMFLOAT3(x, y, z);
}

void CameraClass::setPosition(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(&this->mPosition, position);
}

void CameraClass::updateViewMatrix()
{
	DirectX::XMVECTOR eyePosition = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMVECTOR focusPosition = DirectX::XMLoadFloat3(&this->mLook);
	DirectX::XMVECTOR upDirection = DirectX::XMLoadFloat3(&this->mUp);

	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	//ingen transpose, kanskee inte skall göras om sparas i matrix
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, viewMatrix);
}

void CameraClass::updateProjectionMatrix()
{
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(this->fovAngleY, this->aspectRatio, this->zNear, this->zFar);
	DirectX::XMStoreFloat4x4(&this->mProjectionMatrix, projectionMatrix);
}

void CameraClass::resetAll()
{
	//kan vara rätt //var helpers i exemplet
	this->mPosition = this->standardPosition;
	this->mUp = this->standardUp;
	this->mLook = this->standardLook;
	this->mRight = this->standardRight;
	
	this->updateViewMatrix();
}

void CameraClass::initiate()
{
	this->resetAll();
	this->updateProjectionMatrix();
}

void CameraClass::rotation(DirectX::XMFLOAT4X4 &transform)
{
	DirectX::XMMATRIX tranformMatrix = DirectX::XMLoadFloat4x4(&transform);

	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->mUp);

	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&this->mLook);

	up = DirectX::XMVector3TransformNormal(up, tranformMatrix);
	DirectX::XMVector3Normalize(up);

	look = DirectX::XMVector3TransformNormal(look, tranformMatrix);
	DirectX::XMVector3Normalize(look);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
	DirectX::XMVector3Normalize(right);

	up = DirectX::XMVector3Cross(right, look);

	DirectX::XMStoreFloat3(&mUp, up);
	DirectX::XMStoreFloat3(&mLook, look);
	DirectX::XMStoreFloat3(&mRight, right);
}

D3D11_SUBRESOURCE_DATA CameraClass::getMatricesSubresource()
{
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &matrices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	return data;
}

matrixStruct CameraClass::initiateMatrices()
{
	this->fovAngleY = 3.14 * 0.45;
	this->aspectRatio = 640 / 480;
	this->zNear = 0.1;
	this->zFar = 20;

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(M_PI / 6, 0, 0);
	this->matrices.world = DirectX::XMMatrixTranspose(this->matrices.world);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 0, -2, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 0);

	DirectX::XMVECTOR upDirection;
	upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

	matrices.view = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	DirectX::XMMATRIX temp = matrices.view;
	matrices.view = DirectX::XMMatrixTranspose(temp);

	this->matrices.projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, zNear, zFar);

	temp = matrices.projection;
	this->matrices.projection = DirectX::XMMatrixTranspose(temp);

	DirectX::XMStoreFloat3(&this->mPosition, eyePosition);
	DirectX::XMStoreFloat3(&this->mUp, upDirection);
	DirectX::XMStoreFloat3(&this->mLook, focusPosition);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(upDirection, focusPosition);
	DirectX::XMVector3Normalize(right);

	DirectX::XMStoreFloat3(&this->mRight, right);
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, matrices.view);
	DirectX::XMStoreFloat4x4(&this->mProjectionMatrix, matrices.projection);

	DirectX::XMStoreFloat3(&this->standardPosition, eyePosition);
	DirectX::XMStoreFloat3(&this->standardUp, upDirection);
	DirectX::XMStoreFloat3(&this->standardLook, focusPosition);
	DirectX::XMStoreFloat3(&this->standardRight, right);	// går nog inte pga har ingen right

	return this->matrices;
}

ID3D11Buffer* CameraClass::createConstantBuffer()
{
	D3D11_BUFFER_DESC description;
	ZeroMemory(&description, sizeof(D3D11_BUFFER_DESC));

	description.ByteWidth = sizeof(matrixStruct);
	description.Usage = D3D11_USAGE_DYNAMIC;
	description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	description.MiscFlags = 0;
	description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ID3D11Buffer* pBuffer = 0;
	ZeroMemory(&pBuffer, sizeof(ID3D11Buffer));

	D3D11_SUBRESOURCE_DATA matriceResource = getMatricesSubresource();
	HRESULT hr = this->gDevice->CreateBuffer(&description, &matriceResource, &pBuffer);
	if (FAILED(hr))
	{
		//error
		return 0;
	}
	return (pBuffer);
}

void CameraClass::updateConstantBuffer(ID3D11Buffer * VSConstantBuffer)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	//Låser buffern för GPU:n och hämtar den till CPU
	this->gDeviceContext->Map(VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	this->matrices.projection = DirectX::XMLoadFloat4x4(&this->mProjectionMatrix);

	this->matrices.view = DirectX::XMLoadFloat4x4(&this->mViewMatrix);

	memcpy(dataPtr.pData, &matrices, sizeof(matrices));

	//Ger GPU:n tillgång till datan igen
	this->gDeviceContext->Unmap(VSConstantBuffer, 0);

	this->gDeviceContext->GSSetConstantBuffers(0, 1, &VSConstantBuffer);
}

void CameraClass::update()
{
	DirectX::XMFLOAT2 keyboardAmount = DirectX::XMFLOAT2(0, 0);
	DirectX::XMFLOAT2 mouseAmount = DirectX::XMFLOAT2(0, 0);

	auto kb = this->m_keyboard->GetState();
	if (kb.W)
	{
		keyboardAmount.y = 1.f;
	}
	if (kb.S)
	{
		keyboardAmount.y = -1.f;
	}
	if (kb.A)
	{
		keyboardAmount.x = -1.f;
	}
	if (kb.D)
	{
		keyboardAmount.x = 1.f;
	}

	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMFLOAT3 movement;
	movement.x = (keyboardAmount.x * this->defaultMovementRate); // * elapsed game time
	movement.y = (keyboardAmount.y * this->defaultMovementRate); // * elapsed game time
	movement.z = 0;
	
	DirectX::XMFLOAT3 floatStrafe;
	floatStrafe.x = this->mRight.x * movement.x;
	floatStrafe.y = this->mRight.y * movement.y;
	floatStrafe.z = this->mRight.z * movement.z;

	DirectX::XMVECTOR strafe = DirectX::XMLoadFloat3(&floatStrafe);
	//position += strafe;
	


	auto ms = this->m_mouse->GetState();
	mouseAmount.x = kb.X * this->defaultMouseSensitivity;
	mouseAmount.y = kb.Y * this->defaultMouseSensitivity;
}


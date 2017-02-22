#include "CameraClass.h"

CameraClass::CameraClass(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(1.0f);
	this->defaultMovementRate = 10.0f;
	this->defaultMouseSensitivity = 100.0f;
	this->rotationValue = 0;

	m_keyboard = std::make_unique<DirectX::Keyboard>();
	this->m_mouse = new DirectX::Mouse();

	this->initiateMatrices();
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
}

CameraClass::~CameraClass()
{
	delete this->m_mouse;	 //samma som keyboard
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
	this->aspectRatio = 640.f / 480.f;
	this->zNear = 0.1;
	this->zFar = 200;

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(M_PI / 6, M_PI / 6, 0);
	//this->matrices.world = DirectX::XMMatrixTranspose(this->matrices.world);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 0, -30, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 0);

	DirectX::XMVECTOR upDirection;
	upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

	DirectX::XMVECTOR lookDirecton;
	lookDirecton = DirectX::XMVectorSet(0, 0, 1, 0);

	matrices.view = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);

	DirectX::XMMATRIX temp = matrices.view;
	matrices.view = DirectX::XMMatrixTranspose(temp);

	this->matrices.projection = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, zNear, zFar);

	temp = matrices.projection;
	this->matrices.projection = DirectX::XMMatrixTranspose(temp);

	DirectX::XMStoreFloat3(&this->mPosition, eyePosition);
	DirectX::XMStoreFloat3(&this->mUp, upDirection);
	DirectX::XMStoreFloat3(&this->mLook, lookDirecton);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(upDirection, lookDirecton);
	DirectX::XMVector3Normalize(right);

	DirectX::XMStoreFloat3(&this->mRight, right);

	temp = matrices.view;
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, temp);

	temp = matrices.projection;
	DirectX::XMStoreFloat4x4(&this->mProjectionMatrix, temp);

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

	ID3D11Buffer* pBuffer = nullptr;
	

	D3D11_SUBRESOURCE_DATA matriceResource = getMatricesSubresource();
	HRESULT hr = this->gDevice->CreateBuffer(&description, &matriceResource, &pBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"matrix resource creation failed!", L"error", MB_OK);
		return 0;
	}
	return (pBuffer);
}

void CameraClass::updateConstantBuffer(ID3D11Buffer* VSConstantBuffer)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	this->rotationValue += 0.0005;
	if (this->rotationValue == 100000)
		this->rotationValue = 0;

	//Låser buffern för GPU:n och hämtar den till CPU
	this->gDeviceContext->Map(VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	
	
	DirectX::XMMATRIX temp = this->matrices.world;
	
	temp = DirectX::XMMatrixRotationRollPitchYaw(M_PI/5, rotationValue, 0);//DirectX::XMMatrixRotationRollPitchYaw(this->rotationValue, this->rotationValue, 0);
	 
	this->matrices.world = temp;

	this->matrices.view = DirectX::XMLoadFloat4x4(&this->mViewMatrix);

	memcpy(dataPtr.pData, &matrices, sizeof(matrices));

	//Ger GPU:n tillgång till datan igen
	this->gDeviceContext->Unmap(VSConstantBuffer, 0);

	this->gDeviceContext->GSSetConstantBuffers(0, 1, &VSConstantBuffer);
}

void CameraClass::update()
{
	DirectX::XMFLOAT2 keyboardAmount = DirectX::XMFLOAT2(0, 0);

	//float elapsedTime = gameTime.ElapsedGameTime();	gametid
	
	//keyboard
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
	movement.x = (keyboardAmount.x * this->defaultMovementRate); // * elapsedTime
	movement.y = (keyboardAmount.y * this->defaultMovementRate); // * elapsedTime
	movement.z = 0;

	DirectX::XMFLOAT3 floatStrafe;
	floatStrafe.x = this->mRight.x * movement.x;
	floatStrafe.y = this->mRight.y * movement.x;
	floatStrafe.z = this->mRight.z * movement.x;

	DirectX::XMVECTOR strafe = DirectX::XMLoadFloat3(&floatStrafe);
	DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(strafe));
	DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(strafe));
	DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(strafe));
	
	DirectX::XMFLOAT3 floatForward;
	floatForward.x = this->mLook.x * movement.y;
	floatForward.y = this->mLook.y * movement.y;
	floatForward.z = this->mLook.z * movement.y;

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&floatForward);
	DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(forward));
	DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(forward));
	DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(forward));

	setPosition(position);

	//mus
	DirectX::XMFLOAT2 mouseAmount = DirectX::XMFLOAT2(0, 0);

	auto ms = this->m_mouse->GetState();
	mouseAmount.x = ms.x * this->defaultMouseSensitivity;
	mouseAmount.y = ms.y * this->defaultMouseSensitivity;

	DirectX::XMFLOAT2 rotationAmount = DirectX::XMFLOAT2(0, 0);
	DirectX::XMVECTOR rotationVector = DirectX::XMVectorZero();

	DirectX::XMVectorSetX(rotationVector, mouseAmount.x * this->defaultRotationRate); // * elapsedTime
	DirectX::XMVectorSetY(rotationVector, mouseAmount.y * this->defaultRotationRate); // * elapsedTime

	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&this->mRight);

	float temp = DirectX::XMVectorGetY(rotationVector);

	DirectX::XMMATRIX pitchMatrix = DirectX::XMMatrixRotationAxis(right, temp);
	DirectX::XMMATRIX yawMatrix = DirectX::XMMatrixRotationY(DirectX::XMVectorGetX(rotationVector));

	DirectX::XMMATRIX yawXPitch = DirectX::XMMatrixMultiply(pitchMatrix, yawMatrix);
	DirectX::XMFLOAT4X4 floatYawXPitch;
	DirectX::XMStoreFloat4x4(&floatYawXPitch, yawXPitch);


	this->rotation(floatYawXPitch);

	this->updateViewMatrix();
}

matrixStruct CameraClass::getMatrix() const
{
	return this->matrices;
}


#include "CameraClass.h"

CameraClass::CameraClass(ID3D11Device* gDevice)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(1.0f);
	this->defaultMovementRate = 10.0f;
	this->defaultMouseSensitivity = 100.0f;

	this->initiateMatrices();
	this->gDevice = gDevice;
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
	//kan vara rätt
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

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(M_PI / 6, M_PI / 6, 0);
	//this->matrices.world = DirectX::XMMatrixTranspose(this->matrices.world);

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

	ID3D11Buffer* pBuffer = 0;
	ZeroMemory(&pBuffer, sizeof(ID3D11Buffer));

	D3D11_SUBRESOURCE_DATA matriceResource = getMatricesSubresource();
	HRESULT hr = this->gDevice->CreateBuffer(&description, &matriceResource, &pBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"matrix resource creation failed!", L"error", MB_OK);
		return 0;
	}
	return (pBuffer);
}


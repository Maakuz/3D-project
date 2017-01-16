#include "CameraClass.h"

CameraClass::CameraClass(float FovAngleY, float aspectRatio, float zNear, float zFar,
	DirectX::XMVECTOR position, DirectX::XMVECTOR up,
	DirectX::XMVECTOR look,
	DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(1.0f);
	this->defaultMovementRate = 10.0f;
	this->defaultMouseSensitivity = 100.0f;

	this->defaultFovAngleY = 3.14 * 0.45;
	this->defaultAspectRatio = 640 / 480;
	this->defaultZNear = 0.1;
	this->defaultZFar = 20;

	this->FovAngleY = FovAngleY;
	this->aspectRatio = aspectRatio;
	this->zNear = zNear;
	this->zFar = zFar;
	DirectX::XMStoreFloat3(&this->mPosition, position);
	DirectX::XMStoreFloat3(&this->mUp, up);
	DirectX::XMStoreFloat3(&this->mLook, look);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
	DirectX::XMVector3Normalize(right);

	DirectX::XMStoreFloat3(&this->mRight, right);	// går nog inte pga har ingen right
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, viewMatrix);
	DirectX::XMStoreFloat4x4(&this->mProjectionMatrix, projectionMatrix);

	DirectX::XMStoreFloat3(&this->standardPosition, position);
	DirectX::XMStoreFloat3(&this->standardUp, up);
	DirectX::XMStoreFloat3(&this->standardLook, look);
	DirectX::XMStoreFloat3(&this->standardRight, right);	// går nog inte pga har ingen right
}

CameraClass::~CameraClass()
{

}

void CameraClass::viewProjectionMatrix()
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&this->mViewMatrix);
	DirectX::XMMATRIX projectionMatrix = DirectX::XMLoadFloat4x4(&this->mProjectionMatrix);
	
	return DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);
}

void CameraClass::setPosition(FLOAT X, FLOAT Y, FLOAT Z)
{
	this->position = DirectX::XMVectorSet(x, y, z, 1.0f);
}

void CameraClass::setPosition(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(this->mPosition, position);
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
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(this->FovAngleY, this->AspectRatio, this->zNear, this->zFar);
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

void CameraClass::rotation(DirectX::XMMATRIX transform)
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->mUp);
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&this->mLook);

	up = DirectX::XMVector3TransformNormal(up, transform);
	DirectX::XMVector3Normalize(up);

	look = DirectX::XMVector3TransformNormal(look, transform);
	DirectX::XMVector3Normalize(look);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
	DirectX::XMVector3Normalize(right);

	up = DirectX::XMVector3Cross(right, look);

	DirectX::XMStoreFloat3(&mUp, up);
	DirectX::XMStoreFloat3(&mLook, look);
	DirectX::XMStoreFloat3(&mRight, right);
}

void CameraClass::rotation(DirectX::XMFLOAT4X4 &transform)
{
	DirectX::FXMMATRIX tranformMatrix = DirectX::XMLoadFloat4x4(&transform);
	this->rotation(tranformMatrix);
}

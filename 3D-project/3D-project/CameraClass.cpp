#include "CameraClass.h"

CameraClass::CameraClass(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, HWND window, int width, int height)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(0.1f);

	this->defaultMovementRate = 0.02f;
	this->defaultMouseSensitivity = 0.01f;

	this->rotationValue = 0;

	this->m_keyboard = std::make_unique<DirectX::Keyboard>();
	this->m_mouse = std::make_unique<DirectX::Mouse>();
	this->window = window;
	
	this->width = width;
	this->height = height;
	m_mouse->SetWindow(this->window);

	this->initiateMatrices(width, height);
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;

	this->mDirection = DirectX::XMFLOAT3(0, 0, 1);
	
	this->previousMouseLocation = DirectX::XMINT2(this->width / 2, this->height / 2);
	this->mPitch = 0.f;
	this->mYaw = 0.f;

	this->escapePressed = false;
	this->airRe = true;

	POINT p;
	p.x = this->width / 2;
	p.y = this->height / 2;

	ClientToScreen(this->window, &p);
	SetCursorPos(p.x, p.y);
}

CameraClass::~CameraClass()
{
	
}

void CameraClass::setPosition(float x, float y, float z)
{
	this->mPosition = DirectX::XMFLOAT3(x, y, z);
}

void CameraClass::setPosition(DirectX::XMVECTOR position)
{
	DirectX::XMStoreFloat3(&this->mPosition, position);
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

matrixStruct CameraClass::initiateMatrices(int width, int height)
{
	this->fovAngleY = M_PI * 0.5f; //90� grader fov
	this->aspectRatio = (float)width / (float)height;
	this->zNear = 1.0f;
	this->zFar = 200.f;

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 3, 0, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 1);

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
	}
	return (pBuffer);
}

void CameraClass::updateConstantBuffer(ID3D11Buffer* VSConstantBuffer)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	ZeroMemory(&dataPtr, sizeof(D3D11_MAPPED_SUBRESOURCE));

	this->rotationValue += 0.0005;

	if (this->rotationValue == 100000)
		this->rotationValue = 0;

	//L�ser buffern f�r GPU:n och h�mtar den till CPU
	this->gDeviceContext->Map(VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	
	DirectX::XMMATRIX temp;

	temp = DirectX::XMLoadFloat4x4(&this->mViewMatrix);
	temp = DirectX::XMMatrixTranspose(temp);
	this->matrices.view = temp;

	memcpy(dataPtr.pData, &matrices, sizeof(matrices));

	//Ger GPU:n tillg�ng till datan igen
	this->gDeviceContext->Unmap(VSConstantBuffer, 0);
}

ID3D11Buffer* CameraClass::createCamrePosBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	ID3D11Buffer* pBuffer = nullptr;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &this->mPosition;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &pBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"Camera buffer cration failed", L"error", MB_OK);
	}

	return pBuffer;
}

void CameraClass::updatecameraPosBuffer(ID3D11Buffer* CameraConstantBuffer)
{
	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(D3D11_MAPPED_SUBRESOURCE));

	this->gDeviceContext->Map(CameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->mPosition, sizeof(DirectX::XMFLOAT3));

	this->gDeviceContext->Unmap(CameraConstantBuffer, 0);
}

void CameraClass::kill()
{
	this->gDevice->Release();
	this->gDeviceContext->Release();;
}

bool CameraClass::airResistance()
{
	return this->airRe;
}

void CameraClass::update(float dt)
{
	DirectX::XMFLOAT2 keyboardAmount = DirectX::XMFLOAT2(0, 0);

	//Change defaultMovementRate to change speed
	auto ks = this->m_keyboard->GetState();
	if (ks.W)
	{
		keyboardAmount.y = 1;
	}
	
	if (ks.S)
	{
		keyboardAmount.y = -1;
	}

	if (ks.A)
	{
		keyboardAmount.x = -1;
	}
	
	if (ks.D)
	{
		keyboardAmount.x = 1;
	}

	//this makes the mouse not snap to the middle
	if (ks.Escape)
	{
		this->escapePressed = true;
		ShowCursor(TRUE);
	}

	if (ks.Enter)
	{
		this->escapePressed = false;
		ShowCursor(FALSE);
	}
	if (ks.X)
	{
		this->airRe = true;	
	}
	if (ks.C)
	{
		this->airRe = false;
	}

	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMFLOAT3 movement;

	//removed dt because it felt horrible
	movement.x = (keyboardAmount.x * this->defaultMovementRate);
	movement.y = (keyboardAmount.y * this->defaultMovementRate);
	movement.z = 0;

	DirectX::XMFLOAT3 floatStrafe; //hmm snurrar runt
	floatStrafe.x = this->mRight.x * movement.x;
	floatStrafe.y = this->mRight.y * movement.x;
	floatStrafe.z = this->mRight.z * movement.x;

	DirectX::XMVECTOR strafe = DirectX::XMLoadFloat3(&floatStrafe);

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(strafe)); //bara denna beh�vs?
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(strafe));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(strafe));
	
	//Terrain walking
	DirectX::XMFLOAT3 wUp(0, 1, 0);
	DirectX::XMVECTOR WUp = DirectX::XMLoadFloat3(&wUp);

	DirectX::XMVECTOR RIGHT = DirectX::XMLoadFloat3(&this->mRight);

	DirectX::XMVECTOR TERRAINWALKDIR = DirectX::XMVector3Cross(RIGHT, WUp);

	DirectX::XMFLOAT3 terrainWalkDir;
	DirectX::XMStoreFloat3(&terrainWalkDir, TERRAINWALKDIR);

	DirectX::XMFLOAT3 floatForward;
	floatForward.x = terrainWalkDir.x * movement.y;
	floatForward.y = terrainWalkDir.y * movement.y;
	floatForward.z = terrainWalkDir.z * movement.y;
	/////END

	//NOCLIPMODE (Disable terrain walk)
	//DirectX::XMFLOAT3 floatForward;
	//floatForward.x = this->mDirection.x * movement.y;
	//floatForward.y = this->mDirection.y * movement.y;
	//floatForward.z = this->mDirection.z * movement.y;

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&floatForward);

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(forward));
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(forward));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(forward)); //bara denna beh�vs

	setPosition(position); //sparar den i mPosition
	
	//mus

	DirectX::XMINT2 newMouseLocation = DirectX::XMINT2(0, 0);

	DirectX::XMFLOAT2 rotationAmount = DirectX::XMFLOAT2(0, 0);
	DirectX::XMVECTOR rotationVector = DirectX::XMVectorZero();

	DirectX::XMINT2 deltaMouseMovement = DirectX::XMINT2(0, 0);


	auto ms = this->m_mouse->GetState();

	if (this->escapePressed == false)
	{
		newMouseLocation.x = (ms.x); // nya position
		newMouseLocation.y = (ms.y); 

		deltaMouseMovement.x = newMouseLocation.x - previousMouseLocation.x;
		deltaMouseMovement.y = previousMouseLocation.y - newMouseLocation.y; //y reversed

		//It works, be happy
		if (abs(deltaMouseMovement.x) <= 1)
			deltaMouseMovement.x = 0; 
		
		if (abs(deltaMouseMovement.y) <= 1)
			deltaMouseMovement.y = 0;
		
		previousMouseLocation.x = newMouseLocation.x;
		previousMouseLocation.y = newMouseLocation.y;

		this->mPitch += deltaMouseMovement.y * defaultMouseSensitivity * dt;
		this->mYaw += deltaMouseMovement.x * defaultMouseSensitivity * dt;

		this->mPitch = max(min(this->mPitch, 89.f), -89.f); //89 f�r inte flipp kamera vid 90

		float degToRad = M_PI / 180.f;
		this->mDirection.x = sin(this->mYaw * degToRad) * cos(this->mPitch * degToRad);
		this->mDirection.y = (sin(this->mPitch * degToRad));
		this->mDirection.z = cos(this->mYaw * degToRad) * cos(this->mPitch * degToRad);
		DirectX::XMVECTOR mDirectionVector = DirectX::XMLoadFloat3(&this->mDirection);
		DirectX::XMVector3Normalize(mDirectionVector);
		DirectX::XMStoreFloat3(&this->mDirection, mDirectionVector);

		POINT p;
		p.x = this->width / 2;
		p.y = this->height / 2;

		ClientToScreen(this->window, &p);
		SetCursorPos(p.x, p.y);

		previousMouseLocation = DirectX::XMINT2(width / 2, height / 2);

		ShowCursor(FALSE);

	}

	this->updateViewMatrix(); //view matrixen
}

void CameraClass::updateViewMatrix()
{
	mLook.x = mPosition.x + mDirection.x; //mLook blir positionen man kollar p�
	mLook.y = mPosition.y + mDirection.y;
	mLook.z = mPosition.z + mDirection.z;

	DirectX::XMVECTOR eyePosition = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMVECTOR focusPosition = DirectX::XMLoadFloat3(&this->mLook);
	DirectX::XMVECTOR upDirection = DirectX::XMLoadFloat3(&this->mUp);

	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, viewMatrix);

	//Right
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->mUp);

	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&this->mDirection);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
	right = DirectX::XMVector4Normalize(right);

	DirectX::XMStoreFloat3(&this->mRight, right);
}

matrixStruct CameraClass::getMatrix() const
{
	return this->matrices;
}

DirectX::XMFLOAT3 & CameraClass::getCameraPos()
{
	return this->mPosition;
}


float CameraClass::getNearPlane()
{
	return this->zNear;
}

float CameraClass::getFarPlane()
{
	return this->zFar;
}

float CameraClass::getFovAngleY()
{
	return this->fovAngleY;
}

DirectX::XMFLOAT3 CameraClass::getMDirection()
{
	return this->mDirection;
}

DirectX::XMFLOAT3 CameraClass::getMRight()
{
	return this->mRight;
}
DirectX::XMFLOAT3 CameraClass::getMUp()
{
	return this->mUp;
}
DirectX::XMFLOAT4X4 CameraClass::getProjM() const
{
	return this->mProjectionMatrix;
}
DirectX::XMFLOAT4X4 CameraClass::getViewM() const
{
	return this->mViewMatrix;
}
float CameraClass::getFarZ() const
{
	return this->zFar;
}
float CameraClass::getAspectRatio()
{
	return this->aspectRatio;
}

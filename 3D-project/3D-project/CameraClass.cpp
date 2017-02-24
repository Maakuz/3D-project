#include "CameraClass.h"

CameraClass::CameraClass(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, HWND window, int width, int height)
{
	this->defaultRotationRate = DirectX::XMConvertToRadians(0.1f);
	this->defaultMovementRate = 10.0f;
	this->defaultMouseSensitivity = 0.01f;
	this->rotationValue = 0;

	this->m_keyboard = std::make_unique<DirectX::Keyboard>();
	this->m_mouse = std::make_unique<DirectX::Mouse>();
	this->window = window;
	
	this->width = width;
	this->height = height;
	m_mouse->SetWindow(this->window);

	this->initiateMatrices();
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;

	this->mDirection = DirectX::XMFLOAT3(0, 0, 1);

	
	this->previousMouseLocation = DirectX::XMINT2(this->width / 2, this->height / 2);
	this->mPitch = 0.f;
	this->mYaw = 0.f;

	this->escapePressed = false;

}

CameraClass::~CameraClass()
{
	this->gDevice->Release();
	this->gDeviceContext->Release();
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

matrixStruct CameraClass::initiateMatrices()
{
	this->fovAngleY = M_PI * 0.45;
	this->aspectRatio = 640.f / 480.f;
	this->zNear = 0.1f;
	this->zFar = 20.f;

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(M_PI / 6, M_PI / 6, 0);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 0, -2, 0);

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

	//this->rotationValue += 0.0004;
	if (this->rotationValue == 100000)
		this->rotationValue = 0;

	//Låser buffern för GPU:n och hämtar den till CPU
	this->gDeviceContext->Map(VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	
	
	DirectX::XMMATRIX wTemp = matrices.world;
	
	wTemp = DirectX::XMMatrixRotationRollPitchYaw(this->rotationValue, this->rotationValue, 0);

	this->matrices.world = wTemp;

	DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&this->mViewMatrix);
	temp = DirectX::XMMatrixTranspose(temp);
	this->matrices.view = temp;


	memcpy(dataPtr.pData, &matrices, sizeof(matrices));

	//Ger GPU:n tillgång till datan igen
	this->gDeviceContext->Unmap(VSConstantBuffer, 0);

	this->gDeviceContext->GSSetConstantBuffers(0, 1, &VSConstantBuffer);
}

void CameraClass::update()
{
	DirectX::XMFLOAT2 keyboardAmount = DirectX::XMFLOAT2(0, 0);

	//Add gametime variable

	//keyboard
	auto ks = this->m_keyboard->GetState();
	if (ks.W)
	{
		keyboardAmount.y = 0.00003f;
	}
	
	if (ks.S)
	{
		keyboardAmount.y = -0.00003f;
	}

	if (ks.A)
	{
		keyboardAmount.x = -0.00003f;
	}
	
	if (ks.D)
	{
		keyboardAmount.x = 0.00003f;
	}

	if (ks.Escape)
	{
		this->escapePressed = true;
	}

	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMFLOAT3 movement;
	movement.x = (keyboardAmount.x * this->defaultMovementRate); // * elapsedTime
	movement.y = (keyboardAmount.y * this->defaultMovementRate); // * elapsedTime
	movement.z = 0;

	DirectX::XMFLOAT3 floatStrafe; //hmm snurrar runt
	floatStrafe.x = this->mRight.x * movement.x;
	floatStrafe.y = this->mRight.y * movement.x;
	floatStrafe.z = this->mRight.z * movement.x;

	DirectX::XMVECTOR strafe = DirectX::XMLoadFloat3(&floatStrafe);

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(strafe)); //bara denna behövs?
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(strafe));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(strafe));
	
	DirectX::XMFLOAT3 floatForward;	//funkar?
	floatForward.x = this->mDirection.x * movement.y;
	floatForward.y = this->mDirection.y * movement.y;
	floatForward.z = this->mDirection.z * movement.y;

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&floatForward);

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(forward));
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(forward));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(forward)); //bara denna behövs

	setPosition(position); //sparar den i mPosition
	
	//mus

	DirectX::XMINT2 newMouseLocation = DirectX::XMINT2(0, 0);

	DirectX::XMFLOAT2 rotationAmount = DirectX::XMFLOAT2(0, 0);
	DirectX::XMVECTOR rotationVector = DirectX::XMVectorZero();

	DirectX::XMINT2 deltaMouseMovement = DirectX::XMINT2(0, 0);


	auto ms = this->m_mouse->GetState();

	if (this->escapePressed == false)
	{
		newMouseLocation.x = (ms.x); // nya mnus frra i total
		newMouseLocation.y = (ms.y); //this->defaultMouseSensitivity

		deltaMouseMovement.x = newMouseLocation.x - previousMouseLocation.x;
		deltaMouseMovement.y = previousMouseLocation.y - newMouseLocation.y; //y reversed

		//It works, be happy
		if (abs(deltaMouseMovement.x) <= 1)
			deltaMouseMovement.x = 0; 
		
		if (abs(deltaMouseMovement.y) <= 1)
			deltaMouseMovement.y = 0;
		
		previousMouseLocation.x = newMouseLocation.x;
		previousMouseLocation.y = newMouseLocation.y;

		this->mPitch += deltaMouseMovement.y * defaultMouseSensitivity;
		this->mYaw += deltaMouseMovement.x * defaultMouseSensitivity;

		this->mPitch = max(min(this->mPitch, 89.f), -89.f); //89 för inte flipp kamera vid 90

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
	mLook.x = mPosition.x + mDirection.x; //mLook blir positionen man kollar på
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


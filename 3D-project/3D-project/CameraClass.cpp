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
	/*this->mPosition = DirectX::XMFLOAT3(0, 0, 0);
	this->mUp = DirectX::XMFLOAT3(0, 0, 0);
	this->mLook = DirectX::XMFLOAT3(0, 0, 0);
	this->mRight = DirectX::XMFLOAT3(0, 0, 0);
*/

	

	this->mouseMove = false;
	this->startMouse = DirectX::XMFLOAT2(0, 0);
	this->stopMouse = DirectX::XMFLOAT2(0, 0);
	this->totalMouse = DirectX::XMFLOAT2(0, 0);
	this->previousMouseLocation = DirectX::XMINT2(this->width / 2, this->height / 2);
	this->mPitch = 0.f;
	this->mYaw = 0.f;

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
	mLook.x = mPosition.x + mDirection.x; //mLook blir positionen man kollar p�
	mLook.y = mPosition.y + mDirection.y;
	mLook.z = mPosition.z + mDirection.z;

	DirectX::XMVECTOR eyePosition = DirectX::XMLoadFloat3(&this->mPosition);
	DirectX::XMVECTOR focusPosition = DirectX::XMLoadFloat3(&this->mLook);
	DirectX::XMVECTOR upDirection = DirectX::XMLoadFloat3(&this->mUp);

	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	//ingen transpose, kanskee inte skall g�ras om sparas i matrix
	DirectX::XMStoreFloat4x4(&this->mViewMatrix, viewMatrix);
}

void CameraClass::updateProjectionMatrix()
{
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(this->fovAngleY, this->aspectRatio, this->zNear, this->zFar);
	DirectX::XMStoreFloat4x4(&this->mProjectionMatrix, projectionMatrix);
}

void CameraClass::resetAll()
{
	//kan vara r�tt //var helpers i exemplet
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
	//DirectX::XMMatrixTranspose(tranformMatrix);

	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&this->mUp);

 	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&this->mLook);

	up = DirectX::XMVector3TransformNormal(up, tranformMatrix);
	up = DirectX::XMVector4Normalize(up);

	look = DirectX::XMVector3TransformNormal(look, tranformMatrix);
	look = DirectX::XMVector4Normalize(look);

	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
	right = DirectX::XMVector4Normalize(right);

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
	this->zNear = 0.1f;
	this->zFar = 20.f;

	this->matrices.world = DirectX::XMMatrixRotationRollPitchYaw(M_PI / 6, M_PI / 6, 0);
	//this->matrices.world = DirectX::XMMatrixTranspose(this->matrices.world);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 0, -2, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 0);

	DirectX::XMVECTOR upDirection;
	upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

	DirectX::XMVECTOR lookDirecton;
	lookDirecton = DirectX::XMVectorSet(0, 0, 1, 0);

	matrices.view = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection); //andra kanske skall vara lookDirection?

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
	DirectX::XMStoreFloat3(&this->standardRight, right);	// g�r nog inte pga har ingen right

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

	//L�ser buffern f�r GPU:n och h�mtar den till CPU
	this->gDeviceContext->Map(VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	
	
	DirectX::XMMATRIX wTemp = matrices.world;
	
	wTemp = DirectX::XMMatrixRotationRollPitchYaw(this->rotationValue, this->rotationValue, 0);

	this->matrices.world = wTemp;

	DirectX::XMMATRIX temp = DirectX::XMLoadFloat4x4(&this->mViewMatrix);
	temp = DirectX::XMMatrixTranspose(temp);
	this->matrices.view = temp;


	memcpy(dataPtr.pData, &matrices, sizeof(matrices));

	//Ger GPU:n tillg�ng till datan igen
	this->gDeviceContext->Unmap(VSConstantBuffer, 0);

	this->gDeviceContext->GSSetConstantBuffers(0, 1, &VSConstantBuffer);
}

void CameraClass::update()
{
	DirectX::XMFLOAT2 keyboardAmount = DirectX::XMFLOAT2(0, 0);

	//float elapsedTime = gameTime.ElapsedGameTime();	gametid
	
	//keyboard
	auto ks = this->m_keyboard->GetState();
	if (ks.W)
	{
		keyboardAmount.y = 0.0003f;
	}
	if (ks.S)
	{
		keyboardAmount.y = -0.0003f;
	}
	if (ks.A)
	{
		keyboardAmount.x = -0.0003f;
	}
	if (ks.D)
	{
		keyboardAmount.x = 0.0003f;
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

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(strafe)); //bara denna beh�vs?
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(strafe));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(strafe));
	
	DirectX::XMFLOAT3 floatForward;	//funkar?
	floatForward.x = this->mLook.x * movement.y;
	floatForward.y = this->mLook.y * movement.y;
	floatForward.z = this->mLook.z * movement.y;

	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&floatForward);

	position = DirectX::XMVectorSetX(position, DirectX::XMVectorGetX(position) + DirectX::XMVectorGetX(forward));
	position = DirectX::XMVectorSetY(position, DirectX::XMVectorGetY(position) + DirectX::XMVectorGetY(forward));
	position = DirectX::XMVectorSetZ(position, DirectX::XMVectorGetZ(position) + DirectX::XMVectorGetZ(forward)); //bara denna beh�vs

	setPosition(position); //sparar den i mPosition


						   //Set the look at to the same x coord as the camera

						   //xled borde fungera f�r direction �r bara i z led

						   //this->mDirection.z = this->mDirection.z + 0.00003f; //kanske bara addera?
						   //this->mLook.y = this->mLook.y + this->mPosition.y;
						   //this->mLook.x = this->mPosition.x;



	
	//this->mDirection.z = this->mDirection.z + -0.00003f; // i w och s f�r att kunna g� igenom
	
	//mus

	DirectX::XMINT2 newMouseLocation = DirectX::XMINT2(0, 0);

	DirectX::XMFLOAT2 rotationAmount = DirectX::XMFLOAT2(0, 0);
	DirectX::XMVECTOR rotationVector = DirectX::XMVectorZero();

	DirectX::XMINT2 deltaMouseMovement = DirectX::XMINT2(0, 0);


	auto ms = this->m_mouse->GetState();

	if (this->m_mouse->IsConnected() && ms.leftButton)
	{
		newMouseLocation.x = (ms.x); // nya mnus frra i total
		newMouseLocation.y = (ms.y); //this->defaultMouseSensitivity

		deltaMouseMovement.x = newMouseLocation.x - previousMouseLocation.x;
		deltaMouseMovement.y = previousMouseLocation.y - newMouseLocation.y; //y reversed
		
		previousMouseLocation.x = newMouseLocation.x;
		previousMouseLocation.y = newMouseLocation.y;

		this->mPitch += deltaMouseMovement.y * defaultMouseSensitivity;
		this->mYaw += deltaMouseMovement.x * defaultMouseSensitivity;

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

		//ShowCursor(FALSE);

	}

	this->updateViewMatrix(); //view matrixen


	




	//DirectX::XMFLOAT4 abc = DirectX::XMFLOAT4(0, 0, 0, 0); //bara f�r noll
	//DirectX::XMVECTOR rotationVector = DirectX::XMLoadFloat4(&abc); 

	//rotationVector = DirectX::XMVectorSetX(rotationVector, totalMouseAmount.x * this->defaultRotationRate); // * elapsedTime
	//rotationVector = DirectX::XMVectorSetY(rotationVector, totalMouseAmount.y * this->defaultRotationRate); // * elapsedTime

	//DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&this->mRight);

	//float temp = DirectX::XMVectorGetY(rotationVector);

	//DirectX::XMMATRIX pitchMatrix = DirectX::XMMatrixRotationAxis(right, temp);
	//DirectX::XMMATRIX yawMatrix = DirectX::XMMatrixRotationY(DirectX::XMVectorGetX(rotationVector));

	//DirectX::XMMATRIX pitchXYaw = DirectX::XMMatrixMultiply(pitchMatrix, yawMatrix);
	//DirectX::XMFLOAT4X4 floatPitchXYaw;
	//DirectX::XMStoreFloat4x4(&floatPitchXYaw, pitchXYaw);


	



}

matrixStruct CameraClass::getMatrix() const
{
	return this->matrices;
}


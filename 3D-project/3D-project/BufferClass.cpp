#include "BufferClass.h"

BufferClass::BufferClass(ID3D11Device* gDevice)
{
	initiateMatrices();
	this->gDevice = gDevice;
}

D3D11_SUBRESOURCE_DATA BufferClass::getMatricesSubresource()
{
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &matrices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	return data;
}

matrixStruct BufferClass::initiateMatrices()
{
	float FovAngleY = 3.14 * 0.45;
	float AspectRatio = 640 / 480;
	int zFar = 20;
	double zNear = 0.1;

	this->matrices.world = DirectX::XMMATRIX(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 0, -2, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 0);

	DirectX::XMVECTOR upDirection;
	upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

	this->matrices.view = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	this->matrices.view = DirectX::XMMatrixTranspose(this->matrices.projection);

	this->matrices.projection = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, zNear, zFar);
	this->matrices.projection = DirectX::XMMatrixTranspose(this->matrices.projection);

	return this->matrices;
}

void BufferClass::updateMatrices()
{
	D3D11_MAPPED_SUBRESOURCE pData;
}

ID3D11Buffer* BufferClass::createConstantBuffer(UINT size, D3D11_SUBRESOURCE_DATA* pData)
{
	D3D11_BUFFER_DESC description;
	description.ByteWidth = size;
	description.Usage = D3D11_USAGE_DYNAMIC;
	description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	description.MiscFlags = 0;
	description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = this->gDevice->CreateBuffer(&description, pData, &pBuffer);
	if (FAILED(hr))
	{
		//error
		return 0;
	}
	return (pBuffer);
}

BufferClass::~BufferClass()
{

}
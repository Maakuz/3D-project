#include "BufferClass.h"

BufferClass::BufferClass(ID3D11Device* gDevice)
{
	initiateMatrices();
	this->gDevice = gDevice;
}

D3D11_SUBRESOURCE_DATA BufferClass::getMatricesSubresource()
{
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &matrices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	return data;
}

matrixStruct BufferClass::initiateMatrices()
{
	float FovAngleY = (float)(3.14 * 0.45);
	float AspectRatio = (float)(640 / 480);
	float zFar = 20.0f;
	float zNear = 0.1f;

	this->matrices.world = DirectX::XMMATRIX(
		cos(0), 0, sin(0), 0,
		0, 1, 0, 0,
		-sin(0), 0, cos(0), 0,
		0, 0, 0, 1);

	//this->matrices.world = DirectX::XMMatrixRotationY(0);
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

	this->matrices.projection = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, zNear, zFar);

	temp = matrices.projection;
	this->matrices.projection = DirectX::XMMatrixTranspose(temp);

	return this->matrices;
}

void BufferClass::updateMatrices()
{
	//D3D11_MAPPED_SUBRESOURCE pData;
}

ID3D11Buffer* BufferClass::createConstantBuffer()
{
	D3D11_BUFFER_DESC description;
	ZeroMemory(&description, sizeof(D3D11_BUFFER_DESC));

	description.ByteWidth = sizeof(BufferClass);
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

ID3D11Buffer* BufferClass::createVertexBuffer(std::vector<vertexInfo> *info)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));


	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(info);
		
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = info;

	ID3D11Buffer* tempVertexBuffer;
	ZeroMemory(&tempVertexBuffer, sizeof(ID3D11Buffer));

	this->gDevice->CreateBuffer(&bufferDesc, &data, &tempVertexBuffer); 


	return tempVertexBuffer;
}

BufferClass::~BufferClass()
{
	gDevice->Release();
}
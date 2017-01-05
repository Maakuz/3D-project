#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler(HWND wHandler, int height, int width)
{
	this->gDevice = nullptr;
	this->gDeviceContext = nullptr;
	this->rtvBackBuffer = nullptr;
	this->swapChain = nullptr;
	this->vertexLayout = nullptr;
	this->vertexShader = nullptr;
	this->vertexBuffer = nullptr;

	this->CreateDirect3DContext(wHandler);
	this->setViewPort(height, width);
}

GraphicsHandler::~GraphicsHandler()
{

}

HRESULT GraphicsHandler::CreateDirect3DContext(HWND wHandler)
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = wHandler;
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&desc,
		&swapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		//Depth buffer borde nog hända här


		ID3D11Texture2D* backBuffer = nullptr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

		gDevice->CreateRenderTargetView(backBuffer, NULL, &rtvBackBuffer);
		backBuffer->Release();

		//Lägg in depthviewsaken här i stället för nULL
		gDeviceContext->OMSetRenderTargets(1, &rtvBackBuffer, NULL);
	}
	return hr;
}

void GraphicsHandler::setViewPort(int heigth, int width)
{
	D3D11_VIEWPORT vp;
	vp.Height = (float)heigth;
	vp.Width = (float)width;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	gDeviceContext->RSSetViewports(1, &vp);
}

void GraphicsHandler::createShaders()
{
	ID3DBlob* vsBlob = nullptr;
	D3DCompileFromFile(
		L"VertexShader.hlsl",
		NULL,
		NULL,
		"main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		NULL);

	this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->vertexShader);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->vertexLayout);

	vsBlob->Release();
}

void GraphicsHandler::createTriangleData()
{
	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
	};

	TriangleVertex triangleVertices[6] =
	{
		0.5f, -0.5f, 0.0f,	//v0 pos
		1.0f, 1.0f,	0.0f//v0 color

		- 0.5f, -0.5f, 0.0f,	//v1
		0.0f, 1.0f,	0.0f,//v1 color

		-0.5f, 0.5f, 0.0f, //v2
		0.0f, 0.0f, 1.0f	//v2 color
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &this->vertexBuffer);
}

void GraphicsHandler::render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	this->gDeviceContext->ClearRenderTargetView(rtvBackBuffer, clearColor);
	//Clear depth stencil here

	gDeviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(float) * 6;
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(this->vertexLayout);


	gDeviceContext->Draw(3, 0);

}

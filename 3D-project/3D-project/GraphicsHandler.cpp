#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler(HWND wHandler, int height, int width)
{
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

	///////////////////////////////////////////vetrex shader//////////////////////////////////////////////////////////
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
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

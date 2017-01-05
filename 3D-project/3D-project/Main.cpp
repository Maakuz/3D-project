#include "WindowClass.h"
#include <d3d11.h>


#pragma comment (lib, "d3d11.lib")

IDXGISwapChain* swapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
ID3D11RenderTargetView* rtvBackBuffer = nullptr;



void setViewPort(int heigth, int width);
HRESULT CreateDirect3DContext(HWND wHandler);


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WindowClass windClass(hInstance, 640, 480);

	/*CreateDirect3DContext(wnd);

	setViewPort(640, 480);*/

	

	return windClass.run();
}

HRESULT CreateDirect3DContext(HWND wHandler)
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

void setViewPort(int heigth, int width)
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
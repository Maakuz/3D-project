#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


class GraphicsHandler
{
private:
	IDXGISwapChain* swapChain;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11RenderTargetView* rtvBackBuffer;
	
public:
	GraphicsHandler();
	~GraphicsHandler();

	HRESULT CreateDirect3DContext(HWND wHandler);
	void setViewPort(int heigth, int width);
};
#endif
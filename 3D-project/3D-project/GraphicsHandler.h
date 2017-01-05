#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dcompiler.lib")


class GraphicsHandler
{
private:
	IDXGISwapChain* swapChain;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11RenderTargetView* rtvBackBuffer;
	
	ID3D11VertexShader* vertexShader;
	
public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	HRESULT CreateDirect3DContext(HWND wHandler);
	void setViewPort(int heigth, int width);
	void createShaders();
};
#endif
#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H
#include <string>
#include <fstream>
#include <sstream>
#include "CameraClass.h"
#include "WICTextureLoader.h"




class GraphicsHandler
{
private:
	IDXGISwapChain* swapChain;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11RenderTargetView* rtvBackBuffer;
	
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* vertexLayout;
	ID3D11Buffer* vertexBuffer;
	objectInfo objInfo;
	
	ID3D11Buffer* matrixBuffer;

	//texture things
	ID3D11Resource* textureResoure;
	ID3D11ShaderResourceView* textureView;

	CameraClass* cameraClass;



	objectInfo loadObj();
	void loadMtl();
	void createTexture();
	void createShaders();
	void setViewPort(int heigth, int width);
	void createTriangleData();
	HRESULT CreateDirect3DContext(HWND wHandler);
	void createVertexBuffer();

public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	void render();
};


#endif
#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "BufferClass.h"
#include "WICTextureLoader.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


struct indexInfo
{
	int a1, b1, c1;
	int a2, b2, c2;
	int a3, b3, c3;
};


struct objectInfo
{
	int nrOfVertices;
	int norOfIndexcies;
	
	//VertexInfo is located in BufferClass.h
	std::vector<vertexInfo> vInfo;
	std::vector<indexInfo> iInfo;
};

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
	BufferClass* bufferClass;

	//texture things
	ID3D11Resource* textureResoure;
	ID3D11ShaderResourceView* textureView;
	
public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	HRESULT CreateDirect3DContext(HWND wHandler);
	void setViewPort(int heigth, int width);
	void createShaders();

	void createTexture();

	void createTriangleData();
	void render();
	objectInfo loadObj();
};


#endif
#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H
#include <string>
#include <fstream>
#include <sstream>
#include "CameraClass.h"
#include "WICTextureLoader.h"


const int NROFBUFFERS = 3;


class GraphicsHandler
{
private:
	unsigned int height, width;

	IDXGISwapChain* swapChain;
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	ID3D11RenderTargetView* rtvBackBuffer;

	ID3D11VertexShader* vertexShader;
	ID3D11VertexShader* defferedVertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11PixelShader* defferedPixelShader;

	ID3D11InputLayout* vertexLayout;
	ID3D11InputLayout* defferedVertexLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* defferedVertexBuffer;
	objectInfo objInfo;

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightbuffer;
	ID3D11Buffer* mtlLightbuffer;


	//texture things
	ID3D11Resource* textureResoure;
	ID3D11ShaderResourceView* textureView;

	//Deffered things
	ID3D11Texture2D* renderTargets[NROFBUFFERS];
	ID3D11RenderTargetView* renderTargetViews[NROFBUFFERS];
	ID3D11ShaderResourceView* shaderResourceViews[NROFBUFFERS];

	ID3D11DepthStencilState* disableDepthState;

	ID3D11Texture2D* depthBuffer;
	ID3D11DepthStencilState* dsState;
	ID3D11DepthStencilView* DSV;

	CameraClass* cameraClass;

	lightStruct light;



	void loadObj();
	void loadMtl();
	std::vector<mtlVertex> linkObjNMtl();
	void createTexture();
	void createShaders();
	void setViewPort(int heigth, int width);
	void createTriangleData();
	HRESULT CreateDirect3DContext(HWND wHandler);
	void createVertexBuffer();
	void createDefferedBuffers();
	void createDepthBuffer();
	void createSamplers();
	void createLightBuffer();
	void createMtlLightBuffer();

public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	void render();
	void renderGeometry();
};


#endif
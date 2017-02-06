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
	ID3D11ComputeShader* computeShader;
	ID3D11PixelShader* particlePixel;
	ID3D11VertexShader* particleVertex;
	ID3D11GeometryShader* particleGeometry;
	ID3D11ComputeShader* particleInserter;

	ID3D11InputLayout* vertexLayout;
	ID3D11InputLayout* defferedVertexLayout;
	ID3D11InputLayout* partilceVertexLayout;
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

	//ID3D11RenderTargetView* particleRenderTarget;


	CameraClass* cameraClass;

	lightStruct light;

	ID3D11Buffer* emitterlocation;
	ID3D11Buffer* structBuffer1;
	ID3D11Buffer* structBuffer2;
	ID3D11Buffer* particleCountBuffer;
	ID3D11Buffer* IndirectArgsBuffer;
	ID3D11Buffer* StagingBuffer;
	ID3D11UnorderedAccessView* uav1;
	ID3D11UnorderedAccessView* uav2;
	ID3D11ShaderResourceView* srv1;
	ID3D11ShaderResourceView* srv2;



	void loadObj();
	void loadMtl();
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
	void createParticleBuffers(int nrOfPArticles);
	

public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	void render();
	void renderGeometry();
	void renderParticles();
	void updateParticles();
};


#endif
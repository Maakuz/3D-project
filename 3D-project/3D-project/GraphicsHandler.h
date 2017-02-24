#ifndef GRAPHICSHANDLER_H
#define GRAPHICSHANDLER_H
#include <string>
#include <fstream>
#include <sstream>
#include "CameraClass.h"
#include "WICTextureLoader.h"
#include <time.h>
#include "TerrainHandler.h"


const int NROFBUFFERS = 4;
const UINT startParticleCount = 0;
//makes it so the actual amount of particles in the buffers is used
const UINT UAVFLAG = -1;


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
	ID3D11VertexShader* shadowVertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11PixelShader* defferedPixelShader;
	ID3D11ComputeShader* computeShader;
	

	ID3D11InputLayout* vertexLayout;
	ID3D11InputLayout* defferedVertexLayout;
	ID3D11InputLayout* partilceVertexLayout;

	ID3D11PixelShader* shadowPixelShader;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* defferedVertexBuffer;
	objectInfo objInfo;

	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightbuffer;
	ID3D11Buffer* mtlLightbuffer;
	ID3D11Buffer* lightMatrixBuffer;

	ID3D11SamplerState* sState;

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

	ID3D11Texture2D* shadowDepthBuffer;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;


	CameraClass* cameraClass;
	TerrainHandler* terrainHandler;

	lightStruct light;
	matrixStruct lightMatrices;

	//particle system things
	ID3D11PixelShader* particlePixel;
	ID3D11VertexShader* particleVertex;
	ID3D11GeometryShader* particleGeometry;
	ID3D11ComputeShader* particleInserter;
	ID3D11Buffer* emitterlocation;
	ID3D11Buffer* structBuffer1;
	ID3D11Buffer* structBuffer2;
	ID3D11Buffer* particleCountBuffer;
	ID3D11Buffer* IndirectArgsBuffer;
	ID3D11Buffer* StagingBuffer;
	ID3D11Buffer* deltaTimeBuffer;
	ID3D11UnorderedAccessView* UAVS[2];
	ID3D11UnorderedAccessView* nullUAV;
	ID3D11ShaderResourceView* SRVS[2];
	ID3D11ShaderResourceView* nullSRV;
	ID3D11Resource* particleTexture;
	ID3D11RenderTargetView* nullRTV;
	ID3D11DepthStencilView* nullDSV;

	

	float deltaTime;
	float currentTime;
	float lastInsert;
	


	void loadObj();
	void loadMtl();
	void createTexture();
	void createShaders();
	void setViewPort(int heigth, int width);
	void createTriangleData();
	HRESULT CreateDirect3DContext(HWND wHandler);
	void createVertexBuffer();
	void createDefferedBuffers();
	void createDepthBuffers();
	void createSamplers();
	void createLightBuffer();
	void createMtlLightBuffer();
	void createParticleBuffers(int nrOfPArticles);
	void render();
	void renderGeometry();
	void renderParticles();
	void updateParticles();
	void swapParticleBuffers();
	void particleFirstTimeInit();
	void updateParticleCBuffers(float currentTime);
	
	
	void renderShadows();
	void createLightMatrices();
	void updateLightBuffer();

public:
	GraphicsHandler(HWND wHandler, int height, int width);
	~GraphicsHandler();

	void update(float currentTime);
	

};


#endif
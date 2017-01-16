#ifndef BUFFERCLASS_H
#define BUFFERCLASS_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

#pragma comment (lib, "d3d11.lib")

struct vertexInfo
{

	//vertex positions
	float vpx, vpy, vpz;

	// vertex normals
	float vnx, vny, vnz;

	//uv coords
	float u, v;
};

struct matrixStruct
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};



class BufferClass
{
private:
	matrixStruct matrices;
	ID3D11Device* gDevice;
public:
	BufferClass(ID3D11Device* gDevice);
	virtual ~BufferClass();

	D3D11_SUBRESOURCE_DATA getMatricesSubresource();
	matrixStruct initiateMatrices();
	void updateMatrices();
	ID3D11Buffer* createConstantBuffer();
	ID3D11Buffer* createVertexBuffer(std::vector<vertexInfo> *info);
};



#endif
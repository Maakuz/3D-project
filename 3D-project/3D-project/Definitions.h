#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

struct TriangleVertex
{
	float x, y, z;
	float u, v;
};

struct indexInfo
{
	int a1, b1, c1;
	int a2, b2, c2;
	int a3, b3, c3;
};

struct vertexInfo
{
	//vertex positions
	float vpx, vpy, vpz;

	// vertex normals
	float vnx, vny, vnz;

	//uv coords
	float u, v;
};

struct mtlInfo
{
	
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	float specWeight;

	std::string name;
	std::string texture;
};
struct mtLight
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	/*DirectX::XMFLOAT3 specular;
	float specWeight;*/
};


struct objectInfo
{
	int nrOfVertices;
	int norOfIndexcies;

	//VertexInfo is located in BufferClass.h
	std::vector<vertexInfo> vInfo;
	std::vector<indexInfo> iInfo;
	std::vector<mtlInfo> mInfo;
};





struct matrixStruct
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct lightStruct
{
	DirectX::XMFLOAT4 lightPos;
	DirectX::XMFLOAT4 lightColor;
	DirectX::XMFLOAT4 lightDir;
	DirectX::XMFLOAT4 lightAngle;
	DirectX::XMFLOAT4 lightRange;
};


#endif
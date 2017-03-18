#ifndef TERRAINHANDLER_H
#define TERRAINHANDLER_H
#include "Definitions.h"
#include "WICTextureLoader.h"
#include <fstream>
#include <String>

struct HeightMap { float x, y, z; };

struct FrustumBounds
{
	int xStart, xStop, yStart, yStop;
};

class FrustumTree
{
public:
	FrustumTree* NW;
	FrustumTree* NE;
	FrustumTree* SW;
	FrustumTree* SE;
	AABB boundingVolume;
	VertexInfo* data;
	int vertexCount;

	FrustumTree(AABB boundingVolume, VertexInfo* data, int vertexCount)
	{
		this->NE = nullptr;
		this->SE = nullptr;
		this->NW = nullptr;
		this->SW = nullptr;
		this->boundingVolume = boundingVolume;
		this->data = data;
		this->vertexCount = vertexCount;
	}

	virtual ~FrustumTree()
	{
		delete this->NW;
		delete this->SW;
		delete this->SE;
		delete this->NE;
		delete[] this->data;
	}
};


class TerrainHandler
{	
private:

	int height, width;
	float heightMultiple;
	float camHeightFromTerrain;
	ID3D11Buffer* vertexBuffer;
	ID3D11Resource* res;
	ID3D11ShaderResourceView* srv;
	HeightMap* heightMap;
	VertexInfo* vertices;
	int nrOfVertices;
	int visibleVertices;
	float vertexLength;
	float offsetY;
	FrustumTree* tree;

	void createFrustumTree(int nrOfSplits = 4);
	void _CreateFrustumTree(int nrOfSplits, FrustumBounds bound, AABB box, FrustumTree*& branch, VertexInfo* chunks, int vertexAmount);

	float determinateDeterminant(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b, DirectX::XMFLOAT3& c);
public:
	TerrainHandler(ID3D11Device* gDevice, std::string path);
	virtual ~TerrainHandler();

	void renderTerrain(ID3D11DeviceContext* gDeviceContext);
	void loadHeightMap(ID3D11Device* gDevice, std::string path);
	void createVertices();
	void createVertexBuffer(ID3D11Device* gDevice);
	void setShaderResources(ID3D11DeviceContext* gDeviceContext);
	void kill();
	void walkOnTerrain(DirectX::XMFLOAT3& camPos);
	int getNrOfVertices() const;
	VertexInfo* getVerticies() const;
	FrustumTree* GetFrustumTree() const;
	void updateVertexBuffer(ID3D11DeviceContext* gDeviceContext, FrustumTree* branch);
};


#endif 
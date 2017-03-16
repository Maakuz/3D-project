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

class TerrainHandler
{
private:
	class FrustumTree 
	{
	public:
		FrustumTree* NW;
		FrustumTree* NE;
		FrustumTree* SW;
		FrustumTree* SE;
		AABB boundingVolume;
		VertexInfo* data;

		FrustumTree(AABB boundingVolume, FrustumTree* NW = nullptr, FrustumTree* SW = nullptr, FrustumTree* NE = nullptr, FrustumTree* SE = nullptr)
		{
			this->NE = NE;
			this->SE = SE;
			this->NW = NW;
			this->SW = SW;
			this->boundingVolume = boundingVolume;
			this->data = nullptr;
		}

		virtual ~FrustumTree()
		{
			delete this->NW;
			delete this->SW;
			delete this->SE;
			delete this->NE;
		}
	};


	int height, width;
	float heightMultiple;
	float camHeightFromTerrain;
	ID3D11Buffer* vertexBuffer;
	ID3D11Resource* res;
	ID3D11ShaderResourceView* srv;
	HeightMap* heightMap;
	VertexInfo* vertices;
	int nrOfVertices;
	float vertexLength;
	float offsetY;
	FrustumTree* tree;

	void createFrustumTree(int nrOfSplits = 4);
	void _CreateFrustumTree(int nrOfSplits, FrustumBounds bound, AABB box, FrustumTree* branch, VertexInfo* chunks);

	float determinateDeterminant(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b, DirectX::XMFLOAT3& c);
public:
	TerrainHandler(ID3D11Device* gDevice, std::string path, float heightMultiple = 0);
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
};


#endif 
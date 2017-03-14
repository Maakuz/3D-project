#ifndef TERRAINHANDLER_H
#define TERRAINHANDLER_H
#include "Definitions.h"
#include "WICTextureLoader.h"
#include <fstream>
#include <String>

struct HeightMap { float x, y, z; };


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
	float vertexLength;
	float offsetY;

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
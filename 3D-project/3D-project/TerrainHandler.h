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
	ID3D11Buffer* vertexBuffer;
	HeightMap* heightMap;
	vertexInfo* vertices;
	int nrOfVertices;


public:
	TerrainHandler(ID3D11Device* gDevice, std::string path, float heightMultiple = 0);
	virtual ~TerrainHandler();

	void renderTerrain(ID3D11DeviceContext* gDeviceContext);
	void loadHeightMap(ID3D11Device* gDevice, std::string path);
	void createVertices();
	void createVertexBuffer(ID3D11Device* gDevice);
	void setShaderResources(ID3D11DeviceContext* gDeviceContext);
};


#endif 
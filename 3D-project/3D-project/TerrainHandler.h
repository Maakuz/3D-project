#ifndef TERRAINHANDLER_H
#define TERRAINHANDLER_H
#include "Definitions.h"
#include <fstream>
#include <String>

struct HeightMap { float x, y, z; };


class TerrainHandler
{
private:
	int heigth, width;
	ID3D11Buffer* vertexBuffer;
	HeightMap** heightMap;
	int nrOfVertices;


public:
	TerrainHandler(ID3D11Device* gDevice, std::string path);
	virtual ~TerrainHandler();

	void renderTerrain(ID3D11DeviceContext* gDevice);
	void loadHeightMap(std::string path);
	void createVertices();

};


#endif 
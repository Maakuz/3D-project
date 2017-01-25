#include "TerrainHandler.h"

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path)
{
	this->loadHeightMap(path);
}

TerrainHandler::~TerrainHandler()
{
}

void TerrainHandler::renderTerrain(ID3D11DeviceContext* gDevice)
{
}

void TerrainHandler::loadHeightMap(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (file.is_open())
	{
		PBITMAPFILEHEADER fHeader;
		PBITMAPINFOHEADER bmpInfo;

		char* headers[2];

		headers[0] = new char[sizeof(PBITMAPFILEHEADER)];
		headers[1] = new char[sizeof(PBITMAPINFOHEADER)];

		file.read(headers[0], sizeof(PBITMAPFILEHEADER));
		file.read(headers[1], sizeof(PBITMAPINFOHEADER));

		fHeader = (PBITMAPFILEHEADER)headers[0];
		bmpInfo = (PBITMAPINFOHEADER)headers[1];

		bmpInfo = bmpInfo;
	}

}

void TerrainHandler::createVertices()
{
}

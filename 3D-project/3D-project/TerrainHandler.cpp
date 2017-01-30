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

		char* headers[2] = {nullptr};
		int* colors = nullptr;

		headers[0] = new char[sizeof(PBITMAPFILEHEADER)];
		headers[1] = new char[sizeof(PBITMAPINFOHEADER)];

		file.read(headers[0], sizeof(PBITMAPFILEHEADER));
		file.read(headers[1], sizeof(PBITMAPINFOHEADER));

		fHeader = (PBITMAPFILEHEADER)headers[0];
		bmpInfo = (PBITMAPINFOHEADER)headers[1];



		if (fHeader->bfType == 0x4D42)
		{
			bmpInfo = bmpInfo;
			colors = new int[bmpInfo->biSizeImage];

		}

		delete headers[0];
		delete headers[1];
		delete colors;
	}

}

void TerrainHandler::createVertices()
{
}

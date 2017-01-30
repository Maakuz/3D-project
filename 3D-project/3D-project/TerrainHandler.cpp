#include "TerrainHandler.h"
#include "WICTextureLoader.h"

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path)
{
	this->heigth = 0;
	this->width = 0;
	this->heightMap = nullptr;
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
		BITMAPFILEHEADER* fHeader = nullptr;
		BITMAPINFOHEADER* bmpInfo = nullptr;

		UINT8* headers[2] = {nullptr};
		UINT8* colors = nullptr;

		headers[0] = new UINT8[sizeof(BITMAPFILEHEADER)];
		headers[1] = new UINT8[sizeof(BITMAPINFOHEADER)];

		file.read((char*)headers[0], sizeof(BITMAPFILEHEADER));
		file.read((char*)headers[1], sizeof(BITMAPINFOHEADER));

		fHeader = (BITMAPFILEHEADER*)headers[0];
		bmpInfo = (BITMAPINFOHEADER*)headers[1];


		
		if (fHeader->bfType == 0x4D42)
		{
			
			colors = new UINT8[800 * 600 * 3 * 8];
			this->heightMap = new HeightMap*[bmpInfo->biWidth];
			
			for (size_t i = 0; i < bmpInfo->biWidth; i++)
			{
				this->heightMap[i] = new HeightMap[bmpInfo->biHeight];
			}

			file.seekg(fHeader->bfOffBits);
			file.read((char*)colors, 800 * 600 * 3 * 8);

			//BAH
			for (size_t i = 0; i < bmpInfo->biSizeImage; i+=3)
			{
			}

			int temp = 0;

			for (size_t i = 0; i < bmpInfo->biHeight; i++)
			{
				for (size_t j = 0; j < bmpInfo->biWidth; j++)
				{
					this->heightMap[i][j].z = colors[j * 3 + i];
				}
			}
		}

		delete headers[0];
		delete headers[1];
		delete colors;
	}

}

void TerrainHandler::createVertices()
{
}

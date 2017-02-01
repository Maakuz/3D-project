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


		//Check if it is a .bmp file
		if (fHeader->bfType == 0x4D42)
		{
			
			colors = new UINT8[bmpInfo->biSizeImage];

			//Create a grid of vectors
			this->heightMap = new HeightMap*[bmpInfo->biWidth];
			for (size_t i = 0; i < bmpInfo->biWidth; i++)
			{
				this->heightMap[i] = new HeightMap[bmpInfo->biHeight];
			}

			//Find the end of the header
			file.seekg(fHeader->bfOffBits);

			//Read all the bits into the color array
			file.read((char*)colors, bmpInfo->biSizeImage);

			//Create vectors from the color array
			for (size_t i = 0; i < bmpInfo->biHeight; i++)
			{
				for (size_t j = 0; j < bmpInfo->biWidth; j++)
				{
					this->heightMap[i][j].x = 1;
					this->heightMap[i][j].y = colors[j * 3 + i];
					this->heightMap[i][j].z = 1;

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

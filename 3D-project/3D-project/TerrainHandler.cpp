#include "TerrainHandler.h"
#include "WICTextureLoader.h"

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path)
{
	this->height = 0;
	this->width = 0;
	this->heightMap = nullptr;
	this->loadHeightMap(path);
	this->createVertices();
}

TerrainHandler::~TerrainHandler()
{
	delete[] this->heightMap;
	delete[] this->vertices;
}

void TerrainHandler::renderTerrain(ID3D11DeviceContext* gDevice)
{
}

void TerrainHandler::loadHeightMap(std::string path)
{
	std::ifstream file(path, std::fstream::binary);
	if (file.is_open())
	{
		BITMAPFILEHEADER* fHeader = nullptr;
		BITMAPINFOHEADER* bmpInfo = nullptr;

		UINT8* headers[2] = { nullptr };
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


			this->height = bmpInfo->biHeight;
			this->width = bmpInfo->biWidth;
			
			this->heightMap = new HeightMap[bmpInfo->biSizeImage];
			
			for (size_t i = 0; i < this->width * this->height; i++)
			{
				this->heightMap[i] = { 0 };
			}


			//Find the end of the header
			file.seekg(fHeader->bfOffBits);

			//Read all the bits into the color array
			file.read((char*)colors, bmpInfo->biSizeImage);

			//Create vectors from the color array
			for (size_t i = 0; i < bmpInfo->biSizeImage; i+=3)
			{
					this->heightMap[i].x = 1;
					this->heightMap[i].y = colors[i];
					this->heightMap[i].z = 1;
			}
		}

		file.close();
		delete headers[0];
		delete headers[1];
		delete colors;
	}
}

void TerrainHandler::loadHeightMap(char* path)
{
	FILE* f;
	fopen_s(&f, path, "rb");

	char header[54];
	fread(header, sizeof(char), 54, f);
	
	int height = *(int*)&header[22];
	int width = *(int*)&header[18];

	char* colors = new char[3 * width * height];
	fread(colors, sizeof(char), 3 * width * height, f);

	fclose(f);

	
	delete colors;

}

void TerrainHandler::createVertices()
{
	this->vertices = new vertexInfo[this->height * this->width];

	float offset = 0.3;

	for (size_t i = 0; i < this->height; i++)
	{
		
	}
}

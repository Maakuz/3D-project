#include "TerrainHandler.h"
#include "WICTextureLoader.h"

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path)
{
	this->height = 0;
	this->width = 0;
	this->heightMap = nullptr;
	//this->loadHeightMap(path);
	//Hardcoded stuff because I'm tired of crap


	this->heightMap = new HeightMap[16];
	int heights[16] = 
	{
		0, 0, 50, 50, 
		0, 50, 255, 50,
		50, 255, 75, 0,
		255, 50, 0, 0
	};

	for (size_t i = 0; i < 16; i++)
	{
		heights[i] /= 255.f;
	}

	this->height = 4;
	this->width = 4;

	int count = 0;
	int pos = 0;
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			pos = (4 * j) + i;
			this->heightMap[pos].x = j;
			this->heightMap[pos].y = heights[count];
			this->heightMap[pos].z = i;

			count++;
		}
	}

	this->createVertices();
	this->createVertexBuffer(gDevice);
}

TerrainHandler::~TerrainHandler()
{
	delete[] this->heightMap;
	delete[] this->vertices;
	this->vertexBuffer->Release();
}

void TerrainHandler::renderTerrain(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->Draw(nrOfVertices, 0);
}

void TerrainHandler::loadHeightMap(std::string path)
{
	std::ifstream file(path, std::ios::binary);
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
			
			for (size_t i = 0; i < bmpInfo->biSizeImage; i++)
			{
				colors[i] = 0;
			}


			this->height = bmpInfo->biHeight;
			this->width = bmpInfo->biWidth;
			
			this->heightMap = new HeightMap[this->width * this->height];
			
			for (size_t i = 0; i < this->width * this->height; i++)
			{
				this->heightMap[i] = { 0 };
			}

			//Find the end of the header
			file.seekg(fHeader->bfOffBits);

			//Read all the bits into the color array
			file.read((char*)colors, 65656564656);

			//for testing
			std::ofstream ofile("../test.txt", std::ios::app);
			ofile << colors << std::endl;
			ofile.close();

			//Create vectors from the color array
			int count = 0;
			int pos = 0;
			for (size_t y = 0; y < this->height; y++)
			{
				for (size_t x = 0; x < this->width; x++)
				{
					pos = (height * y) + x;
					this->heightMap[pos].x = x;
					this->heightMap[pos].y = colors[count];
					this->heightMap[pos].z = y;
					
					count += 9;
				}
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

	this->nrOfVertices = (this->height - 1) * (this->width - 1) * 6;
	this->vertices = new vertexInfo[this->nrOfVertices];

	float offset = 2.f;
	int count = 0;

	//Once for every quad plus the iterations we skip at the corner of the heightmap
	for (size_t i = 0; i < (this->nrOfVertices / 6) + this->height - 2; i++)
	{
		if (count % (6 * (this->width - 1)) == 0 && count != 0)
			i++;

		//Create the first
		this->vertices[count] =
		{
			this->heightMap[i].x * offset,
			this->heightMap[i].y,
			this->heightMap[i].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,
			//Vertex position
			0.f, 0.f
		};
		count++;

		//Create one to the right of the first
		this->vertices[count] =
		{
			this->heightMap[i + 1].x * offset,
			this->heightMap[i + 1].y,
			this->heightMap[i + 1].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 0.f
		};
		count++;

		//Create one under the first
		this->vertices[count] =
		{
			this->heightMap[i + this->width].x * offset,
			this->heightMap[i + this->width].y,
			this->heightMap[i + this->width].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			0.f, 1.f
		};
		count++;

		//Create the first vertex in the second triangle (Same as 2)
		this->vertices[count] =
		{
			this->heightMap[i + 1].x * offset,
			this->heightMap[i + 1].y,
			this->heightMap[i + 1].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 0.f
		};
		count++;

		//Create one under the last one
		this->vertices[count] =
		{
			this->heightMap[i + 1 + this->width].x * offset,
			this->heightMap[i + 1 + this->width].y,
			this->heightMap[i + 1 + this->width].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 1.f
		};
		count++;

		//Create one to the left of the last one (same as vertex 3)
		this->vertices[count] =
		{
			this->heightMap[i + this->width].x * offset,
			this->heightMap[i + this->width].y,
			this->heightMap[i + this->width].z * offset,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			0.f, 1.f
		};
		count++;
	}


}

void TerrainHandler::createVertexBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(vertexInfo) * this->nrOfVertices;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->vertices;

	gDevice->CreateBuffer(&desc, &data, &this->vertexBuffer);
}

void TerrainHandler::setShaderResources(ID3D11DeviceContext* gDeviceContext)
{
	UINT32 stride = sizeof(vertexInfo);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

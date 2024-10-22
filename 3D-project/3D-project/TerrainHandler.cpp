#include "TerrainHandler.h"

void TerrainHandler::createFrustumTree(int nrOfSplits)
{
	AABB box;
	box.p0.x = this->vertices[0].vpx;
	box.p0.y = this->offsetY;
	box.p0.z = this->vertices[0].vpz;

	box.p1.x = this->vertices[this->nrOfVertices-1].vpx;
	box.p1.y = this->heightMultiple + this->offsetY;
	box.p1.z = this->vertices[this->nrOfVertices-1].vpz;


	FrustumBounds bound;
	bound.xStart = 0;
	bound.yStart = 0;
	bound.xEnd = this->width - 1;
	bound.yEnd = this->height - 1;


	this->tree = new FrustumTree(this->nrOfVertices);

	this->tree->boundingVolume = box;

	this->_CreateFrustumTree(nrOfSplits, bound, this->tree);
}

void TerrainHandler::_CreateFrustumTree(int nrOfSplits, FrustumBounds bound, FrustumTree*& branch)
{
	if (nrOfSplits > 0)
	{
		//*****************************************************NW**********************************************************
		//Get the bounds of the terrain in this region
		FrustumBounds nextBound;
		nextBound.xStart = bound.xStart;
		nextBound.yStart = bound.yStart;
		nextBound.xEnd = bound.xStart + ((bound.xEnd - bound.xStart) / 2);
		nextBound.yEnd = bound.yStart + ((bound.yEnd - bound.yStart) / 2);

		this->createBranchData(nextBound, branch->NW);

		this->_CreateFrustumTree(nrOfSplits - 1, nextBound, branch->NW);


		//*****************************************************NE**********************************************************
		//Get the bounds of the terrain in this region
		nextBound.xStart = bound.xStart + ((bound.xEnd - bound.xStart) / 2);
		nextBound.yStart = bound.yStart;
		nextBound.xEnd = bound.xEnd;
		nextBound.yEnd = bound.yStart + ((bound.yEnd - bound.yStart) / 2);

		this->createBranchData(nextBound, branch->NE);

		this->_CreateFrustumTree(nrOfSplits - 1, nextBound, branch->NE);

		//*****************************************************SW**********************************************************
		//Get the bounds of the terrain in this region
		nextBound.xStart = bound.xStart;
		nextBound.yStart = bound.yStart + ((bound.yEnd - bound.yStart) / 2);
		nextBound.xEnd = bound.xStart + ((bound.xEnd - bound.xStart) / 2);
		nextBound.yEnd = bound.yEnd;

		this->createBranchData(nextBound, branch->SW);

		this->_CreateFrustumTree(nrOfSplits - 1, nextBound, branch->SW);


		//******************************************************SE*********************************************************
		//Get the bounds of the terrain in this region
		nextBound.xStart = bound.xStart + ((bound.xEnd - bound.xStart) / 2);
		nextBound.yStart = bound.yStart + ((bound.yEnd - bound.yStart) / 2);
		nextBound.xEnd = bound.xEnd;
		nextBound.yEnd = bound.yEnd;

		this->createBranchData(nextBound, branch->SE);

		this->_CreateFrustumTree(nrOfSplits - 1, nextBound, branch->SE);
	}
}

void TerrainHandler::createBranchData(FrustumBounds bound, FrustumTree *& branch)
{
	int width = bound.xEnd - bound.xStart;
	int height = bound.yEnd - bound.yStart;
	int verticeCount = width * height * 6;

	branch = new FrustumTree(verticeCount);

	branch->data = new VertexInfo[verticeCount];

	//Magic formula to find a chunk of vertices in a 1D array.
	//If split into terms it can be seen as:

	//j: iterator that adds one to the count
	//i * (this->width - 1) * 6: Adds the number of vertices on a single row times i
	//(bound.xStart * 6): Start position in x-axis
	//((this->width - 1) * 6 * bound.yStart): Start position in y-axis

	for (size_t i = 0; i < height; i++)
	{
		for (size_t j = 0; j < width * 6; j++)
		{
			branch->data[(i * width * 6) + j] = this->vertices[(i * (this->width - 1) * 6) + j + (bound.xStart * 6) + ((this->width - 1) * 6 * bound.yStart)];
		}
	}

	//The box to test the frustum against
	AABB nextBox;
	nextBox.p0.x = branch->data[0].vpx;
	nextBox.p0.y = this->offsetY;
	nextBox.p0.z = branch->data[0].vpz;

	nextBox.p1.x = branch->data[verticeCount - 1].vpx;
	nextBox.p1.y = this->heightMultiple + this->offsetY;
	nextBox.p1.z = branch->data[verticeCount - 1].vpz;

	branch->boundingVolume = nextBox;
}

float TerrainHandler::determinateDeterminant(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b, DirectX::XMFLOAT3& c)
{
	return ((a.x * b.y * c.z) - (a.x * b.z * c.y)) +
		((a.y * b.z * c.x) - (a.y * b.x * c.z)) +
		((a.z * b.x * c.y) - (a.z * b.y * c.x));
}

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path)
{
	this->height = 0;
	this->width = 0;
	this->heightMultiple = 50;
	this->camHeightFromTerrain = 3.f;
	this->heightMap = nullptr;
	this->vertexLength = 0.3f;
	this->offsetY = -35;
	this->loadHeightMap(gDevice, path);
	
	this->createVertices();
	this->createFrustumTree(4);
	this->visibleVertices = this->nrOfVertices;

	this->createVertexBuffer(gDevice);

	HRESULT hr = DirectX::CreateWICTextureFromFile(
		gDevice,
		L"../resource/Maps/kung.png",
		&this->res,
		&this->srv);
	if (FAILED(hr))
	{
		MessageBox(0, L"Texture creation failed", L"error", MB_OK);
	}

}

TerrainHandler::~TerrainHandler()
{
}

void TerrainHandler::renderTerrain(ID3D11DeviceContext* gDeviceContext)
{
	gDeviceContext->Draw(this->visibleVertices, 0);
}

void TerrainHandler::loadHeightMap(ID3D11Device* gDevice, std::string path)
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
			this->height = bmpInfo->biHeight;
			this->width = bmpInfo->biWidth;

			int size = this->width * this->height * 3;

			colors = new UINT8[size];

			for (size_t i = 0; i < size; i++)
			{
				colors[i] = 0;
			}


			this->heightMap = new HeightMap[this->width * this->height];

			for (size_t i = 0; i < this->width * this->height; i++)
			{
				this->heightMap[i] = { 0 };
			}

			//Find the end of the header
			file.seekg(fHeader->bfOffBits);

			//Read all the bits into the color array
			file.read((char*)colors, size);

			file.close();

			//Create vectors from the color array
			int count = 0;
			int pos = 0;
			for (int j = 0; j < this->height; j++)
			{
				for (int i = 0; i < this->width; i++)
				{
					pos = (this->height * j) + i;
					this->heightMap[pos].x = j;
					this->heightMap[pos].y = colors[count];
					this->heightMap[pos].z = i;

					count+=3;
				}
			}

			//Normalize heightmap
			for (size_t i = 0; i < this->width * this->height; i++)
			{
				this->heightMap[i].y /= 255;
			}
		}

		delete headers[0];
		delete headers[1];
		delete colors;
	}
}

void TerrainHandler::createVertices()
{

	this->nrOfVertices = (this->height - 1) * (this->width - 1) * 6;
	this->vertices = new VertexInfo[this->nrOfVertices];
	
	//Initializing
	for (size_t i = 0; i < this->nrOfVertices; i++)
	{
		this->vertices[i] = { 0 };
	}

	float offsetX = ((this->width - 1) * this->vertexLength) / 2.f;
	float offsetZ = ((this->height - 1) * this->vertexLength) / 2.f;

	int count = 0;

	DirectX::XMFLOAT3 edge1;
	DirectX::XMFLOAT3 edge2;
	DirectX::XMFLOAT3 normal;

	DirectX::XMVECTOR temp1;
	DirectX::XMVECTOR temp2;
	DirectX::XMVECTOR norm;


	//Once for every quad plus the iterations we skip at the corner of the heightmap
	for (size_t i = 0; i < (this->nrOfVertices / 6) + this->height - 2; i++)
	{
		if (count % (6 * (this->width - 1)) == 0 && count != 0)
			i++;

		//Create the first
		this->vertices[count] =
		{
			this->heightMap[i].x * this->vertexLength - offsetX,
			this->heightMap[i].y * this->heightMultiple + this->offsetY,
			this->heightMap[i].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,
			//Vertex position
			0.f, 0.f
		};
		count++;

		//Create one to the right of the first
		this->vertices[count] =
		{
			this->heightMap[i + 1].x * this->vertexLength - offsetX,
			this->heightMap[i + 1].y * this->heightMultiple + this->offsetY,
			this->heightMap[i + 1].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 0.f
		};
		count++;

		//Create one under the first
		this->vertices[count] =
		{
			this->heightMap[i + this->width].x * this->vertexLength - offsetX,
			this->heightMap[i + this->width].y * this->heightMultiple + this->offsetY,
			this->heightMap[i + this->width].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			0.f, 1.f
		};
		
		//Determin the normal for the first triangle
		edge1 = DirectX::XMFLOAT3(
			this->vertices[count - 2].vpx - this->vertices[count].vpx,
			this->vertices[count - 2].vpy - this->vertices[count].vpy,
			this->vertices[count - 2].vpz - this->vertices[count].vpz);

		edge2 = DirectX::XMFLOAT3(
			this->vertices[count - 1].vpx - this->vertices[count].vpx,
			this->vertices[count - 1].vpy - this->vertices[count].vpy,
			this->vertices[count - 1].vpz - this->vertices[count].vpz);

		temp1 = DirectX::XMLoadFloat3(&edge1);
		temp2 = DirectX::XMLoadFloat3(&edge2);

		norm = DirectX::XMVector3Cross(temp1, temp2);

		DirectX::XMStoreFloat3(&normal, norm);
		
		//Set the normal for the first three vertices and the mtl
		for (int i = 0; i < 3; i++)
		{
			this->vertices[count - i].vnx = normal.x;
			this->vertices[count - i].vny = normal.y;
			this->vertices[count - i].vnz = normal.z;

			this->vertices[count - i].mtlType = 0;
		}

		count++;
		
		//Create the first vertex in the second triangle (Same as 2)
		this->vertices[count] =
		{
			this->heightMap[i + 1].x * this->vertexLength - offsetX,
			this->heightMap[i + 1].y * this->heightMultiple + this->offsetY,
			this->heightMap[i + 1].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 0.f
		};
		count++;

		//Create one under the last one
		this->vertices[count] =
		{
			this->heightMap[i + 1 + this->width].x * this->vertexLength - offsetX,
			this->heightMap[i + 1 + this->width].y * this->heightMultiple + this->offsetY,
			this->heightMap[i + 1 + this->width].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			1.f, 1.f
		};
		count++;

		//Create one to the left of the last one (same as vertex 3)
		this->vertices[count] =
		{
			this->heightMap[i + this->width].x * this->vertexLength - offsetX,
			this->heightMap[i + this->width].y * this->heightMultiple + this->offsetY,
			this->heightMap[i + this->width].z * this->vertexLength - offsetZ,

			//Normals goes here later
			0.f, 1.f, 0.f,

			//Vertex position
			0.f, 1.f
		};

		//Determin the normal for the second triangle
		edge1 = DirectX::XMFLOAT3(
			this->vertices[count - 2].vpx - this->vertices[count].vpx,
			this->vertices[count - 2].vpy - this->vertices[count].vpy,
			this->vertices[count - 2].vpz - this->vertices[count].vpz);

		edge2 = DirectX::XMFLOAT3(
			this->vertices[count - 1].vpx - this->vertices[count].vpx,
			this->vertices[count - 1].vpy - this->vertices[count].vpy,
			this->vertices[count - 1].vpz - this->vertices[count].vpz);

		temp1 = DirectX::XMLoadFloat3(&edge1);
		temp2 = DirectX::XMLoadFloat3(&edge2);

		norm = DirectX::XMVector3Cross(temp1, temp2);

		DirectX::XMStoreFloat3(&normal, norm);

		//Set the normal for the first three vertices
		for (int i = 0; i < 3; i++)
		{
			this->vertices[count - i].vnx = normal.x;
			this->vertices[count - i].vny = normal.y;
			this->vertices[count - i].vnz = normal.z;

			this->vertices[count - i].mtlType = 0;
		}

		count++;
	}

}

void TerrainHandler::createVertexBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(VertexInfo) * this->nrOfVertices;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->vertices;

	HRESULT hr = gDevice->CreateBuffer(&desc, &data, &this->vertexBuffer);

	if (FAILED(hr))
	{
		MessageBox(0, L"Terrain VB creation failed", L"error", MB_OK);
		exit(-1);
	}
}

void TerrainHandler::setShaderResources(ID3D11DeviceContext* gDeviceContext)
{
	UINT32 stride = sizeof(VertexInfo);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	gDeviceContext->PSSetShaderResources(0, 1, &this->srv);
	gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TerrainHandler::kill()
{
	ULONG test = 0;
	delete[] this->heightMap;
	delete this->tree;
	delete[] this->vertices;

	test = this->vertexBuffer->Release();
	test = this->res->Release();
	test = this->srv->Release();
}

void TerrainHandler::walkOnTerrain(DirectX::XMFLOAT3& camPos)
{
	//Someone messed up and made z into x in the terrain so this is the game we're playing now
	//HARDCODED VALUE UGH
	float norCamX = camPos.z + ((this->width - 1) * this->vertexLength) / 2.f;
	float norCamZ = camPos.x + ((this->height - 1) * this->vertexLength) / 2.f;

	norCamX /= this->vertexLength;
	norCamZ /= this->vertexLength;

	int roundedX = (int)(norCamX + 0.5f);
	int roundedZ = (int)(norCamZ + 0.5f);
	
	int whatVertex = ((this->width - 1) * 6 * (int)norCamZ) + ((int)norCamX * 6);

	//If we are outside the array we don't bother check
	if (norCamX >= 0 && norCamZ >= 0 && whatVertex <= this->nrOfVertices)
	{
		VertexInfo* v1 = &this->vertices[whatVertex];
		VertexInfo* v2 = nullptr;
		VertexInfo* v3 = nullptr;

		//Might need to create a drawing to explain this sorcery

		//The first vertex is always the top left in the quad and the two other will 
		//be the ones adjacent to the coordinate

		//if the rounded value and the normal as an int is the same we are in the NW corner
		if (roundedX == (int)norCamX && roundedZ == (int)norCamZ)
		{
			//Right
			v2 = &this->vertices[whatVertex + 1];
			//under
			v3 = &this->vertices[whatVertex + 2];
		}

		//If Z is different we're SW 
		else if (roundedX == (int)norCamX && roundedZ != (int)norCamZ)
		{
			//Under
			v2 = &this->vertices[whatVertex + 2];
			//Right
			v3 = &this->vertices[whatVertex + 4];
		}

		//If X is different we're NE
		else if (roundedX != (int)norCamX && roundedZ == (int)norCamZ)
		{
			//right
			v2 = &this->vertices[whatVertex + 1];
			//down-right
			v3 = &this->vertices[whatVertex + 4];
		}

		//If both is different we're SE 
		else
		{
			//down
			v2 = &this->vertices[whatVertex + 2];
			//down-right
			v3 = &this->vertices[whatVertex + 4];

			//We also needs to change v1 since it is NW and that is not adjacent
			v1 = &this->vertices[whatVertex + 1];
		}

		DirectX::XMFLOAT3 e0(v1->vpx - v2->vpx, v1->vpy - v2->vpy, v1->vpz - v2->vpz);
		DirectX::XMFLOAT3 e1(v3->vpx - v2->vpx, v3->vpy - v2->vpy, v3->vpz - v2->vpz);
		DirectX::XMFLOAT3 s(camPos.x - v2->vpx, camPos.y - v2->vpy, camPos.z - v2->vpz);

		DirectX::XMFLOAT3 dir(0, 1, 0);

		float divisionDet = this->determinateDeterminant(dir, e0, e1);


		float det1 = determinateDeterminant(s, e0, e1);
		float det2 = determinateDeterminant(dir, s, e1);
		float det3 = determinateDeterminant(dir, e0, s);

		divisionDet = 1 / divisionDet;


		float t = det1 * divisionDet;
		float u = det2 * divisionDet;
		float v = det3 * divisionDet;

		if (u + v <= 1 && u > 0 && v > 0)
		{
			camPos.y += -1 * t;
			camPos.y += this->camHeightFromTerrain;
		}

	}
}

int TerrainHandler::getNrOfVertices() const
{
	return this->nrOfVertices;
}

VertexInfo* TerrainHandler::getVerticies() const
{
	return this->vertices;
}

FrustumTree* TerrainHandler::GetFrustumTree() const
{
	return this->tree;
}

void TerrainHandler::updateVertexBuffer(ID3D11DeviceContext* gDeviceContext, VertexInfo* info, int size)
{
	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(D3D11_MAPPED_SUBRESOURCE));

	this->visibleVertices = size;


	gDeviceContext->Map(this->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, info, sizeof(VertexInfo) * size);

	gDeviceContext->Unmap(this->vertexBuffer, 0);
}

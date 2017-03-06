#include "TerrainHandler.h"

float TerrainHandler::determinateDeterminant(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b, DirectX::XMFLOAT3& c)
{
	return ((a.x * b.y * c.z) - (a.x * b.z * c.y)) +
		((a.y * b.z * c.x) - (a.y * b.x * c.z)) +
		((a.z * b.x * c.y) - (a.z * b.y * c.x));
}

TerrainHandler::TerrainHandler(ID3D11Device* gDevice, std::string path, float heightMultiple)
{
	this->height = 0;
	this->width = 0;
	this->heightMultiple = heightMultiple;
	this->heightMap = nullptr;
	this->loadHeightMap(path);
	this->vertexLength = 2.f;

	this->createVertices();
	this->createVertexBuffer(gDevice);

	HRESULT hr = DirectX::CreateWICTextureFromFile(
		gDevice,
		L"../resource/Maps/kung.png",
		&this->res,
		&this->srv);
	if (FAILED(hr))
	{
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}

}

TerrainHandler::~TerrainHandler()
{
	delete[] this->heightMap;
	delete[] this->vertices;
	this->vertexBuffer->Release();
}

void TerrainHandler::renderTerrain(ID3D11DeviceContext* gDeviceContext)
{
	//gDeviceContext->Draw(this->nrOfVertices, 0);
	gDeviceContext->DrawIndexed(this->nrOfindices, 0, 0);
}

void TerrainHandler::loadHeightMap(std::string path)
{

	//Depricated maybe //No, not quite
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

	this->nrOfVertices = this->height * this->width;
	this->nrOfindices = (this->height - 1) * (this->width - 1) * 6;

	this->vertices = new VertexInfo[this->nrOfVertices];
	this->indices = new int[this->nrOfindices];
	
	//Initialize
	for (size_t i = 0; i < this->nrOfVertices; i++)
	{
		this->vertices[i] = { 0 };
	}

	for (size_t i = 0; i < this->nrOfindices; i++)
	{
		this->indices[i] = 0;
	}

	float offsetX = ((this->width - 1) * this->vertexLength) / 2.f;
	float offsetY = 10.f;
	float offsetZ = ((this->height - 1) * this->vertexLength) / 2.f;
	int count = 0;

	DirectX::XMFLOAT3 edge1;
	DirectX::XMFLOAT3 edge2;
	DirectX::XMFLOAT3 normal;

	DirectX::XMVECTOR temp1;
	DirectX::XMVECTOR temp2;
	DirectX::XMVECTOR norm;

	for (size_t i = 0; i < this->nrOfVertices; i++)
	{
		this->vertices[i] = 
		{
			this->heightMap[i].x * this->vertexLength - offsetX,
			this->heightMap[i].y * this->heightMultiple - offsetY,
			this->heightMap[i].z * this->vertexLength - offsetZ,

			//This normal will be overwritten later
			0.f, 1.f, 0.f,
			//Same for this UV
			0.f, 0.f
		};
	}
	
	//Once for every quad plus the iterations we skip at the corner of the heightmap
	for (size_t i = 0; i < (this->nrOfVertices / 6) + this->height - 2; i++)
	{
		if (count % (6 * (this->width - 1)) == 0 && count != 0)
			i++;

		//Create the first
		this->indices[count++] = i;

		//Create one to the right of the first
		this->indices[count++] = i + 1;

		//Create one under the first
		this->indices[count++] = i + this->width;
		
		//Create the first vertex in the second triangle (Same as 2)
		this->indices[count++] = i + 1;

		//Create one under the last one
		this->indices[count++] = i + 1 + this->width;

		//Create one to the left of the last one (same as vertex 3)
		this->indices[count++] = i + this->width;
	}



	//Set the normals and uv's for each vertex

	for (size_t i = 0; i < this->nrOfindices- 2; i+= 3)
	{
		//Determin the normal for the first triangle
		edge1 = DirectX::XMFLOAT3(
		this->vertices[this->indices[i + 2]].vpx - this->vertices[this->indices[i]].vpx,
		this->vertices[this->indices[i + 2]].vpy - this->vertices[this->indices[i]].vpy,
		this->vertices[this->indices[i + 2]].vpz - this->vertices[this->indices[i]].vpz);

		edge2 = DirectX::XMFLOAT3(
		this->vertices[this->indices[i + 2]].vpx - this->vertices[this->indices[i + 1]].vpx,
		this->vertices[this->indices[i + 2]].vpy - this->vertices[this->indices[i + 1]].vpy,
		this->vertices[this->indices[i + 2]].vpz - this->vertices[this->indices[i + 1]].vpz);

		temp1 = DirectX::XMLoadFloat3(&edge1);
		temp2 = DirectX::XMLoadFloat3(&edge2);

		norm = DirectX::XMVector3Cross(temp1, temp2);

		DirectX::XMStoreFloat3(&normal, norm);

		//Set the normal for the first three vertices and the mtl
		for (int j = 0; j < 3; j++)
		{
			this->vertices[this->indices[i + j]].vnx = normal.x;
			this->vertices[this->indices[i + j]].vny = normal.y;
			this->vertices[this->indices[i + j]].vnz = normal.z;

			this->vertices[this->indices[i + j]].mtlType = 0;
		}

		
	}

	//This exist for test purposes only.
	for (size_t i = 0; i < this->nrOfindices; i++)
	{
		this->vertices[indices[i]] = this->vertices[indices[i]];
	}
}

void TerrainHandler::createVertexBuffer(ID3D11Device* gDevice)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(VertexInfo) * this->nrOfVertices;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->vertices;

	gDevice->CreateBuffer(&desc, &data, &this->vertexBuffer);

	//Index buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(int) * this->nrOfVertices;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = this->indices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&bufferDesc, &InitData, &this->indexBuffer);
}

void TerrainHandler::setShaderResources(ID3D11DeviceContext* gDeviceContext)
{
	UINT32 stride = sizeof(VertexInfo);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	gDeviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &this->srv);
	gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TerrainHandler::walkOnTerrain(DirectX::XMFLOAT3& camPos)
{
	float norCamX = camPos.x + ((this->width - 1) * this->vertexLength) / 2.f;
	float norCamZ = camPos.z + ((this->height - 1) * this->vertexLength) / 2.f;

	norCamX /= this->vertexLength;
	norCamZ /= this->vertexLength;

	int roundedX = (int)(norCamZ + 0.5f);
	int roundedZ = (int)(norCamX + 0.5f);

	int whatVertex = (this->width * roundedX) + roundedZ;

	//If we are outside the array we don't bother check
	if (norCamX >= 0 && norCamZ >= 0 && whatVertex <= this->nrOfVertices)
	{
		VertexInfo* v1 = &this->vertices[whatVertex];
		VertexInfo* v2 = nullptr;
		VertexInfo* v3 = nullptr;

		//if the rounded value and the normal as an int is the same we are in the NW corner
		//Might need to create a drawing to explain this sorcery
		if (roundedX == (int)norCamX && roundedZ == (int)norCamZ)
		{
			//Right
			v2 = &this->vertices[whatVertex + 6];
			//under
			v3 = &this->vertices[whatVertex + (this->width - 1) * 6];
		}

		//If Z is different we're SW
		else if (roundedX == (int)norCamX && roundedZ != (int)norCamZ)
		{
			v2 = &this->vertices[whatVertex - ((this->width - 1) * 6) + 6];
			v3 = &this->vertices[whatVertex - ((this->width - 1) * 6)];
		}

		//If X is different we're NE
		else if (roundedX != (int)norCamX && roundedZ == (int)norCamZ)
		{
			v2 = &this->vertices[whatVertex - 6];
			v3 = &this->vertices[whatVertex + ((this->width - 1) * 6) - 6];
		}

		//If both is different we're SE
		else
		{
			v2 = &this->vertices[whatVertex - ((this->width - 1) * 6)];
			v3 = &this->vertices[whatVertex - 6];
		}

		DirectX::XMFLOAT3 e0(v1->vpx - v2->vpx, v1->vpy - v2->vpy, v1->vpz - v2->vpz);
		DirectX::XMFLOAT3 e1(v3->vpx - v2->vpx, v3->vpy - v2->vpy, v3->vpz - v2->vpz);
		DirectX::XMFLOAT3 s(camPos.x - v2->vpx, camPos.y - v2->vpy, camPos.z - v2->vpz);

		DirectX::XMFLOAT3 dir(0, 1, 0);

		float divisionDet = this->determinateDeterminant(dir, e0, e1);

		if (abs(divisionDet) > DBL_EPSILON)
		{
			float det1 = determinateDeterminant(s, e0, e1);
			float det2 = determinateDeterminant(dir, s, e1);
			float det3 = determinateDeterminant(dir, e0, s);

			divisionDet = 1 / divisionDet;


			float t = det1 * divisionDet;
			float u = det2 * divisionDet;
			float v = det3 * divisionDet;

			if (u + v <= 1 && u > 0 && v > 0 && u <= 1 && v <= 1)
			{
				camPos.y += -1 * t;
				camPos.y += 1.f;
			}
		}
	}
}

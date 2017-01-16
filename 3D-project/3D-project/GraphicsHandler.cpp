#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler(HWND wHandler, int height, int width)
{
	this->gDevice = nullptr;
	this->gDeviceContext = nullptr;
	this->rtvBackBuffer = nullptr;
	this->swapChain = nullptr;
	this->vertexLayout = nullptr;
	this->vertexShader = nullptr;
	this->vertexBuffer = nullptr;
	this->pixelShader = nullptr;
	this->bufferClass = nullptr;

	this->CreateDirect3DContext(wHandler);
	this->setViewPort(height, width);
	this->createShaders();
	//this->createTriangleData();
	this->createTexture();
	this->objInfo = this->loadObj();

	this->vertexBuffer = this->bufferClass->createVertexBuffer(&this->objInfo.vInfo);
	UINT32 vertexSize = sizeof(vertexInfo);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);

	this->matrixBuffer = bufferClass->createConstantBuffer();
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
}

GraphicsHandler::~GraphicsHandler()
{
	delete bufferClass;
	vertexBuffer->Release();
	rtvBackBuffer->Release();
	swapChain->Release();
	vertexLayout->Release();
	vertexShader->Release();
	pixelShader->Release();
	gDevice->Release();
	gDeviceContext->Release();
}

HRESULT GraphicsHandler::CreateDirect3DContext(HWND wHandler)
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = wHandler;
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG, //Sätt till null på skoldatorrerneA D3D11_CREATE_DEVICE_DEBUG
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&desc,
		&swapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		//Depth buffer borde nog hända här


		ID3D11Texture2D* backBuffer = nullptr;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			MessageBox(0, L"getBuffer failed", L"error", MB_OK);
			return hr;
		}

		hr = gDevice->CreateRenderTargetView(backBuffer, NULL, &rtvBackBuffer);
		if (FAILED(hr))
		{
			MessageBox(0, L"RTV creation failed", L"error", MB_OK);
			return hr;
		}
		backBuffer->Release();

		//Lägg in depthviewsaken här i stället för nULL
		gDeviceContext->OMSetRenderTargets(1, &rtvBackBuffer, NULL);

		this->bufferClass = new BufferClass(this->gDevice);
	}
	return hr;
}

void GraphicsHandler::setViewPort(int heigth, int width)
{
	D3D11_VIEWPORT vp;
	vp.Height = (float)heigth;
	vp.Width = (float)width;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	gDeviceContext->RSSetViewports(1, &vp);
}

void GraphicsHandler::createShaders()
{
	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->vertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, L"vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->vertexLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"PixelShader.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"pixel shader creation failed", L"error", MB_OK);
	}


}

void GraphicsHandler::createTexture()
{

	DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../resource/Maps/skin.tif", &this->textureResoure, &this->textureView);

	this->gDeviceContext->PSSetShaderResources(0, 1, &this->textureView);
}

void GraphicsHandler::createTriangleData()
{
	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
		float u, v;
		//float pad;
	};

	TriangleVertex triangleVertices[6] =
	{
		0.5f, -0.5f, 0.0f,	//v0 pos
		0.0f, 0.0f,	1.0f,   //v0 color
		1.0f, 1.0f,

		-0.5f, -0.5f, 0.0f,	//v1
		0.0f, 0.0f,	1.0f,   //v1 color
		0.0f, 1.0f,

		-0.5f, 0.5f, 0.0f, //v2
		0.0f, 0.0f, 1.0f, //v2 color
		0.0f,  0.0f,

		//t2
		-0.5f, 0.5f, 0.0f,	//v0 pos
		0.0f, 0.0f,	1.0f,   //v0 color
		0.0f, 0.0f,

		0.5f, 0.5f, 0.0f,	//v1
		0.0f, 0.0f,	1.0f,   //v1 color
		1.0f, 0.0f,

		0.5f, -0.5f, 0.0f, //v2
		0.0f, 0.0f, 1.0f, //v2 color
		1.0f, 1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	this->gDevice->CreateBuffer(&bufferDesc, &data, &this->vertexBuffer);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);


	this->matrixBuffer = bufferClass->createConstantBuffer();
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
}

objectInfo GraphicsHandler::loadObj()
{
	std::string fileName("../resource/apple.obj"), identifier;
	std::string line;
	std::ifstream file(fileName);
	objectInfo objInfo;
	if (file.is_open() == false)
	{
		MessageBox(0, L"obj file failed to open", L"error", MB_OK);
	}
	else
	{


		std::istringstream inputString;
		struct vPos
		{
			float x, y, z;
		};
		struct vNor
		{
			float x, y, z;
		};
		struct UV
		{
			float u, v;
		};


		vPos vpTemp;
		vNor vnTemp;
		UV uvTemp;
		indexInfo fTemp;
		std::string temp;
		std::string temp1;
		std::string temp2;
		std::string temp3;
		int i = 0;
		int counter = 0;
		bool cont = true;



		std::vector<vPos> vp;
		std::vector<vNor> vn;
		std::vector<UV> uv;
		std::vector<indexInfo> f;



		while (std::getline(file, line))
		{
			inputString.str(line);
			//fills vertex pos
			if (line.substr(0, 2) == "v ")
			{
				inputString >> identifier >>
					vpTemp.x >> vpTemp.y >> vpTemp.z;
				vp.push_back(vpTemp);
			}
			//fills vertex normals
			else if (line.substr(0, 2) == "vn")
			{
				inputString >> identifier >>
					vnTemp.x >> vnTemp.y >> vnTemp.z;
				vn.push_back(vnTemp);
			}
			//fills texcoords
			else if (line.substr(0, 2) == "vt")
			{
				inputString >> identifier >>
					uvTemp.u >> uvTemp.v;
				uv.push_back(uvTemp);
			}
			//fills faces
			else if (line.substr(0, 2) == "f ")
			{
				inputString >> identifier >> 
					temp1 >> temp2 >> temp3;
				temp1 += 'e';
				temp2 += 'e';
				temp3 += 'e';
				while (cont)
				{
					if (temp1[i] != '/' && temp1[i] != 'e')
					{
						temp += temp1[i];
					}
					else
					{
						if (counter == 0)
						{
							fTemp.a1 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 1)
						{
							fTemp.b1 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 2)
						{
							fTemp.c1 = std::stoi(temp);
							counter++;
							temp = "";
							cont = false;
						}
					}
					i++;
				}
				i = 0;
				counter = 0;
				cont = true;
				while (cont)
				{
					if (temp2[i] != '/' && temp2[i] != 'e')
					{
						temp += temp2[i];
					}
					else
					{
						if (counter == 0)
						{
							fTemp.a2 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 1)
						{
							fTemp.b2 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 2)
						{
							fTemp.c2 = std::stoi(temp);
							counter++;
							temp = "";
							cont = false;
						}
					}
				
					i++;
				}
				i = 0;
				counter = 0;
				cont = true;

				while (cont)
				{
					if (temp3[i] != '/' && temp3[i] != 'e')
					{
						temp += temp3[i];
					}
					else
					{
						if (counter == 0)
						{
							fTemp.a3 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 1)
						{
							fTemp.b3 = std::stoi(temp);
							counter++;
							temp = "";
						}
						else if (counter == 2)
						{
							fTemp.c3 = std::stoi(temp);
							counter++;
							temp = "";
							cont = false;
						}
					}
					i++;
				}
				cont = true;
				i = 0;
				counter = 0;
				f.push_back(fTemp);
				inputString = std::istringstream();
			}
			
		}
		file.close();

		//fill objInfo with the data
		vertexInfo tempVInfo;

	
		for (size_t i = 0; i < f.size(); i++)
		{
			// vertex 1 in face i
			tempVInfo.vpx = vp.at(f.at(i).a1 - 1).x;
			tempVInfo.vpy = vp.at(f.at(i).a1 - 1).y;
			tempVInfo.vpz = vp.at(f.at(i).a1 - 1).z;

			
			tempVInfo.vnx = vn.at(f.at(i).b1 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).b1 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).b1 - 1).z;
			

			//todo THIS IS FUCKED!
			if (uv.size() > i)
			{
				tempVInfo.u = uv.at(f.at(i).c1 - 1).u;
				tempVInfo.v = uv.at(f.at(i).c1 - 1).v;
			}
			else
			{
				tempVInfo.u = -1;
				tempVInfo.v = -1;
			}
		

			objInfo.vInfo.push_back(tempVInfo);

			// vertex 2 in face i
			tempVInfo.vpx = vp.at(f.at(i).a2 - 1).x;
			tempVInfo.vpy = vp.at(f.at(i).a2 - 1).y;
			tempVInfo.vpz = vp.at(f.at(i).a2 - 1).z;

			
			tempVInfo.vnx = vn.at(f.at(i).b2 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).b2 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).b2 - 1).z;
			
			//todo THIS IS FUCKED!
			if (uv.size() > i)
			{
				tempVInfo.u = uv.at(f.at(i).c2 - 1).u;
				tempVInfo.v = uv.at(f.at(i).c2 - 1).v;
			}
			else
			{
				tempVInfo.u = -1;
				tempVInfo.v = -1;
			}

			objInfo.vInfo.push_back(tempVInfo);

			// vertex 3 in face i
			tempVInfo.vpx = vp.at(f.at(i).a3 - 1).x;
			tempVInfo.vpy = vp.at(f.at(i).a3 - 1).y;
			tempVInfo.vpz = vp.at(f.at(i).a3 - 1).z;

			
			tempVInfo.vnx = vn.at(f.at(i).b3 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).b3 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).b3 - 1).z;
			
			
			// todo THIS IS FUCKED!
			if (uv.size() > i)
			{
				tempVInfo.u = uv.at(f.at(i).c3 - 1).u;
				tempVInfo.v = uv.at(f.at(i).c3 - 1).v;
			}
			else
			{
				tempVInfo.u = -1;
				tempVInfo.v = -1;
			}

			objInfo.vInfo.push_back(tempVInfo);

		}

		objInfo.nrOfVertices = (int)(vp.size());
		objInfo.norOfIndexcies = (int)(f.size());

		objInfo.iInfo = f;
	}
	return objInfo;
}

void GraphicsHandler::render()
{
	float clearColor[] = { 0, 0, 0, 1 };
	this->gDeviceContext->ClearRenderTargetView(rtvBackBuffer, clearColor);
	//Clear depth stencil here

	gDeviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(this->pixelShader, nullptr, 0);


	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(this->vertexLayout);


	gDeviceContext->Draw(6, 0);
	this->swapChain->Present(0, 0);
}

#include "GraphicsHandler.h"

GraphicsHandler::GraphicsHandler(HWND wHandler, int height, int width)
{
	this->height = height;
	this->width = width;

	this->gDevice = nullptr;
	this->gDeviceContext = nullptr;
	this->rtvBackBuffer = nullptr;
	this->swapChain = nullptr;
	this->vertexLayout = nullptr;
	this->vertexShader = nullptr;
	this->vertexBuffer = nullptr;
	this->pixelShader = nullptr;
	this->cameraClass = nullptr;
	this->depthBuffer = nullptr;
	//this->particleRenderTarget = nullptr;
	this->structBuffer1 = nullptr;
	this->structBuffer2 = nullptr;
	this->particleCountBuffer = nullptr;
	this->IndirectArgsBuffer = nullptr;
	this->StagingBuffer = nullptr;
	this->uav1 = nullptr;
	this->uav2 = nullptr;
	this->srv1 = nullptr;
	this->srv2 = nullptr;
	this->particleInserter = nullptr;

	this->computeShader = nullptr;
	this->particleGeometry = nullptr;
	this->particlePixel = nullptr;
	this->particleVertex = nullptr;
	this->partilceVertexLayout = nullptr;

	this->defferedPixelShader = nullptr;
	this->defferedVertexShader = nullptr;
	this->dsState = nullptr;
	this->defferedVertexBuffer = nullptr;
	this->DSV = nullptr;
	this->mtlLightbuffer = nullptr;
	this->emitterlocation = nullptr;


	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->renderTargets[i] = nullptr;
		this->renderTargetViews[i] = nullptr;
		this->shaderResourceViews[i] = nullptr;
	}

	this->CreateDirect3DContext(wHandler);
	this->setViewPort(height, width);

	this->cameraClass = new CameraClass(this->gDevice, this->gDeviceContext);

	this->createShaders();
	this->createTexture();
	this->createSamplers();
	this->loadMtl();
	this->loadObj();

	this->createTriangleData();
	this->createLightBuffer();

	this->createLightBuffer();
	this->createVertexBuffer();
	this->createMtlLightBuffer();
	this->createParticleBuffers(512);




	//Constant buffer till vertex shader
	this->matrixBuffer = this->cameraClass->createConstantBuffer();
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
}

GraphicsHandler::~GraphicsHandler()
{
	delete this->cameraClass;
	this->vertexBuffer->Release();
	this->rtvBackBuffer->Release();
	this->swapChain->Release();
	this->vertexLayout->Release();

	this->defferedPixelShader->Release();
	this->vertexShader->Release();
	this->pixelShader->Release();
	this->particleGeometry->Release();
	this->particleVertex->Release();
	this->particlePixel->Release();
	this->partilceVertexLayout->Release();
	//this->particleRenderTarget->Release();
	this->emitterlocation->Release();
	this->structBuffer1->Release();
	this->structBuffer2->Release();
	this->particleCountBuffer->Release();
	this->IndirectArgsBuffer->Release();
	//this->StagingBuffer->Release();
	this->uav1->Release();
	this->uav2->Release();
	this->srv1->Release();
	this->srv2->Release();
	this->particleInserter->Release();

	this->depthBuffer->Release();
	this->dsState->Release();
	this->DSV->Release();
	this->textureResoure->Release();
	this->textureView->Release();
	this->defferedVertexLayout->Release();
	this->defferedVertexShader->Release();
	this->disableDepthState->Release();
	this->matrixBuffer->Release();
	this->lightbuffer->Release();
	this->mtlLightbuffer->Release();

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->renderTargets[i]->Release();
		this->renderTargetViews[i]->Release();
		this->shaderResourceViews[i]->Release();
	}

	this->gDevice->Release();
	this->gDeviceContext->Release();

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
	desc.BufferDesc.Height = this->height;
	desc.BufferDesc.Width = this->width;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL, //Sätt till null på skoldatorrerneA D3D11_CREATE_DEVICE_DEBUG
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
		this->createDepthBuffer();
		this->createDefferedBuffers();

		ID3D11Texture2D* backBuffer = nullptr;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			MessageBox(0, L"swap chain getBuffer failed", L"error", MB_OK);
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
		gDeviceContext->OMSetRenderTargets(1, &this->rtvBackBuffer, this->DSV);

	}
	else
	{
		MessageBox(0, L"remove debug flag", L"error", MB_OK);
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->vertexLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	//Deffered VS
	ID3DBlob* dvsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"DefferedVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&dvsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"dvsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(dvsBlob->GetBufferPointer(), dvsBlob->GetBufferSize(), NULL, &this->defferedVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, L"deffered vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDescDeffered[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MTLNR", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(inputDescDeffered, ARRAYSIZE(inputDescDeffered), dvsBlob->GetBufferPointer(), dvsBlob->GetBufferSize(), &this->defferedVertexLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"input desc creation failed", L"error", MB_OK);
	}

	dvsBlob->Release();

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

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->pixelShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();

	ID3DBlob *dpsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"DefferedPS.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		0,
		0,
		&dpsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"Deffered psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(dpsBlob->GetBufferPointer(), dpsBlob->GetBufferSize(), nullptr, &this->defferedPixelShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"Deffered pixel shader creation failed", L"error", MB_OK);
	}

	dpsBlob->Release();

	ID3D10Blob *csBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ComputeShader.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&csBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &this->computeShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute Shader creation failed", L"error", MB_OK);
	}
	csBlob->Release();

	ID3D10Blob *icsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ComputeShader.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&icsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"inserter compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(icsBlob->GetBufferPointer(), icsBlob->GetBufferSize(), nullptr, &this->particleInserter);
	if (FAILED(hr))
	{
		MessageBox(0, L"inserter compute Shader creation failed", L"error", MB_OK);
	}
	icsBlob->Release();

	ID3DBlob* pvsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"particleVertex.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&pvsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"pvsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(pvsBlob->GetBufferPointer(), pvsBlob->GetBufferSize(), NULL, &this->particleVertex);

	if (FAILED(hr))
	{
		MessageBox(0, L"particle vertex shader creation failed", L"error", MB_OK);
	}

	//this is wrong should be nothing
	D3D11_INPUT_ELEMENT_DESC paricleInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(paricleInputDesc, ARRAYSIZE(paricleInputDesc), pvsBlob->GetBufferPointer(), pvsBlob->GetBufferSize(), &this->partilceVertexLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L" particle input desc creation failed", L"error", MB_OK);
	}

	pvsBlob->Release();

	ID3DBlob* pgsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ParticleGeometry.hlsl",
		NULL,
		NULL,
		"main",
		"gs_5_0",
		0,
		0,
		&pgsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle geometry shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateGeometryShader(pgsBlob->GetBufferPointer(), pgsBlob->GetBufferSize(), nullptr, &this->particleGeometry);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle Geometry shader creation failed", L"error", MB_OK);
		pgsBlob->Release();
	}

	ID3DBlob *ppsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ParticlePixel.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		0,
		0,
		&ppsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"particle psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(ppsBlob->GetBufferPointer(), ppsBlob->GetBufferSize(), nullptr, &this->particlePixel);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle pixel shader creation failed", L"error", MB_OK);
	}

	ppsBlob->Release();

}

void GraphicsHandler::createTexture()
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../resource/Maps/kung.png", &this->textureResoure, &this->textureView);
	if (FAILED(hr))
	{
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}

}

void GraphicsHandler::createTriangleData()
{

	TriangleVertex triangleVertices[6] =
	{
		1.f, -1.f, 0.0f,	//v0 pos
		1.0f, 1.0f,

		-1.f, -1.f, 0.0f,	//v1
		0.0f, 1.0f,

		-1.f, 1.f, 0.0f, //v2
		0.0f,  0.0f,

		//t2
		-1.f, 1.f, 0.0f,	//v0 pos
		0.0f, 0.0f,

		1.f, 1.f, 0.0f,	//v1
		1.0f, 0.0f,

		1.f, -1.f, 0.0f, //v2
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


	this->matrixBuffer = cameraClass->createConstantBuffer();
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
}

void GraphicsHandler::loadObj()
{
	std::string fileName("../resource/Cube.obj"), identifier;
	std::string line;
	std::ifstream file(fileName);

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
		std::string mtlName;
		int mtlNr = 0;
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
				inputString = std::istringstream();
			}
			//fills vertex normals
			else if (line.substr(0, 2) == "vn")
			{
				inputString >> identifier >>
					vnTemp.x >> vnTemp.y >> vnTemp.z;
				vn.push_back(vnTemp);
				inputString = std::istringstream();
			}
			//fills texcoords
			else if (line.substr(0, 2) == "vt")
			{
				inputString >> identifier >>
					uvTemp.u >> uvTemp.v;
				uv.push_back(uvTemp);
				inputString = std::istringstream();
			}
			//sets mtl
			else if (line.substr(0, 7) == "usemtl ")
			{
				inputString >> identifier >> mtlName;
				for (size_t i = 0; i < this->objInfo.nrOfMaterials; i++)
				{
					if (mtlName == this->objInfo.mInfo.at(i).name)
					{
						mtlNr = this->objInfo.mInfo.at(i).mtlType;
					}
				}

				inputString = std::istringstream();
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
				fTemp.mtlNr = mtlNr;
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

			//uv 1 in face i
			tempVInfo.u = uv.at(f.at(i).b1 - 1).u;
			tempVInfo.v = uv.at(f.at(i).b1 - 1).v;

			//normal 1 in face i 
			tempVInfo.vnx = vn.at(f.at(i).c1 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).c1 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).c1 - 1).z;




			tempVInfo.mtlType = f.at(i).mtlNr;
			this->objInfo.vInfo.push_back(tempVInfo);

			// vertex 2 in face i
			tempVInfo.vpx = vp.at(f.at(i).a2 - 1).x;
			tempVInfo.vpy = vp.at(f.at(i).a2 - 1).y;
			tempVInfo.vpz = vp.at(f.at(i).a2 - 1).z;

			//uv 2 in face i
			tempVInfo.u = uv.at(f.at(i).b2 - 1).u;
			tempVInfo.v = uv.at(f.at(i).b2 - 1).v;

			//normal 1 in face i 
			tempVInfo.vnx = vn.at(f.at(i).c2 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).c2 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).c2 - 1).z;




			tempVInfo.mtlType = f.at(i).mtlNr;
			this->objInfo.vInfo.push_back(tempVInfo);

			// vertex 3 in face i
			tempVInfo.vpx = vp.at(f.at(i).a3 - 1).x;
			tempVInfo.vpy = vp.at(f.at(i).a3 - 1).y;
			tempVInfo.vpz = vp.at(f.at(i).a3 - 1).z;

			//uv 3 in face i
			tempVInfo.u = uv.at(f.at(i).b3 - 1).u;
			tempVInfo.v = uv.at(f.at(i).b3 - 1).v;

			//normal 1 in face i 
			tempVInfo.vnx = vn.at(f.at(i).c3 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).c3 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).c3 - 1).z;


			tempVInfo.mtlType = f.at(i).mtlNr;
			this->objInfo.vInfo.push_back(tempVInfo);

		}

		this->objInfo.nrOfVertices = (int)(vp.size());
		this->objInfo.norOfIndexcies = (int)(f.size());

		this->objInfo.iInfo = f;
	}

}

void GraphicsHandler::loadMtl()
{
	std::string fileName("../resource/Cube.mtl"), identifier;
	std::string line;
	std::string prev = "";
	std::ifstream file(fileName);
	this->objInfo.nrOfMaterials = 0;
	mtlInfo tempMInfo;
	tempMInfo.mtlType = 0;
	bool cont = true;
	bool eof = false;

	if (file.is_open() == false)
	{
		MessageBox(0, L"obj file failed to open", L"error", MB_OK);
	}
	else
	{
		std::istringstream inputString;


		while (!eof)
		{


			if (prev.substr(0, 7) == "newmtl ")
			{
				line = prev;
				prev = "";
			}
			else
			{
				std::getline(file, line);
			}
			inputString.str(line);

			if (line.substr(0, 7) == "newmtl ")
			{
				inputString >> identifier >> tempMInfo.name;
				cont = true;
				tempMInfo.mtlType++;
				inputString = std::istringstream();
				while (cont)
				{

					if (std::getline(file, line))
					{
						inputString.str(line);
						if (line.substr(0, 7) != "newmtl ")
						{
							if (line.substr(0, 2) == "Ka")
							{
								inputString >> identifier >>
									tempMInfo.ambient.x >> tempMInfo.ambient.y >> tempMInfo.ambient.z;
								inputString = std::istringstream();
							}

							if (line.substr(0, 2) == "Kd")
							{
								inputString >> identifier >>
									tempMInfo.diffuse.x >> tempMInfo.diffuse.y >> tempMInfo.diffuse.z;
								inputString = std::istringstream();
							}
							if (line.substr(0, 2) == "Ks")
							{
								inputString >> identifier >>
									tempMInfo.specular.x >> tempMInfo.specular.y >> tempMInfo.specular.z;
								inputString = std::istringstream();
							}
							if (line.substr(0, 2) == "Ns")
							{
								inputString >> identifier >> tempMInfo.specWeight;
								inputString = std::istringstream();
							}
							if (line.substr(0, 7) == "map_Ks ")
							{
								inputString >> identifier >> tempMInfo.texture;
								inputString = std::istringstream();
							}
						}
						else
						{
							this->objInfo.mInfo.push_back(tempMInfo);
							this->objInfo.nrOfMaterials++;
							cont = false;
						}

					}
					else
					{
						prev = line;
						this->objInfo.mInfo.push_back(tempMInfo);
						this->objInfo.nrOfMaterials++;
						cont = false;
						eof = true;
					}
				}
			}
		}

	}

}

void GraphicsHandler::createMtlLightBuffer()
{
	mtLight *ml = new mtLight[this->objInfo.nrOfMaterials];

	for (size_t i = 0; i < this->objInfo.nrOfMaterials; i++)
	{
		ml[i].ambient = this->objInfo.mInfo.at(i).ambient;
		ml[i].diffuse = this->objInfo.mInfo.at(i).diffuse;
		ml[i].specular = this->objInfo.mInfo.at(i).specular;
		ml[i].specular.w = this->objInfo.mInfo.at(i).specWeight;
	}


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(mtLight);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &ml;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->mtlLightbuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"mtl light buffer creation failed!", L"error", MB_OK);
	}

	delete[] ml;
}

void GraphicsHandler::createParticleBuffers(int nrOfPArticles)
{
	DirectX::XMFLOAT4 emitterLocation(0.0f, 0.0f, 0.0f, 1.0f);
	Particle *particles = new Particle[nrOfPArticles];

	for (size_t i = 0; i < nrOfPArticles; i++)
	{
		particles[i].position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		particles[i].velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		particles[i].age = 0.0f;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &emitterLocation;

	//creates emitter lacation constant buffer
	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->emitterlocation);
	if (FAILED(hr))
	{
		MessageBox(0, L"emitter creation failed", L"error", MB_OK);
	}

	data.pSysMem = particles;

	D3D11_BUFFER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(D3D11_BUFFER_DESC));

	sDesc.ByteWidth = nrOfPArticles * sizeof(Particle);
	sDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	sDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sDesc.StructureByteStride = sizeof(Particle);
	sDesc.Usage = D3D11_USAGE_DEFAULT;
	sDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	//creates 2 structured buffers who si going to be used as append/consume buffers for the particle system
	hr = this->gDevice->CreateBuffer(&sDesc, &data, &this->structBuffer1);
	if (FAILED(hr))
	{
		MessageBox(0, L"struct buffer 1 creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateBuffer(&sDesc, &data, &this->structBuffer2);
	if (FAILED(hr))
	{
		MessageBox(0, L"struct buffer 2 creation failed", L"error", MB_OK);
	}
	delete[] particles;


	
	D3D11_BUFFER_UAV uav;

	uav.FirstElement = 0;
	uav.NumElements = nrOfPArticles;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer = uav;

	

	D3D11_BUFFER_SRV srv;
	ZeroMemory(&srv, sizeof(D3D11_BUFFER_SRV));
	srv.FirstElement = 0;
	srv.NumElements = nrOfPArticles;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer = srv;

	//uav and srv for first buffer
	hr = this->gDevice->CreateUnorderedAccessView(this->structBuffer1, &uavDesc, &this->uav1);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 1 creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateShaderResourceView(this->structBuffer1, &srvDesc, &this->srv1);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 1 creation failed", L"error", MB_OK);
	}

	//uav and srv for second buffer
	hr = this->gDevice->CreateUnorderedAccessView(this->structBuffer2, &uavDesc, &this->uav2);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 2 creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateShaderResourceView(this->structBuffer2, &srvDesc, &this->srv2);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 2 creation failed", L"error", MB_OK);
	}

	//create constant buffer who holds the nr of particles
	D3D11_BUFFER_DESC nrBDesc;
	ZeroMemory(&nrBDesc, sizeof(D3D11_BUFFER_DESC));
	nrBDesc.ByteWidth = 4 * sizeof(UINT);
	nrBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	nrBDesc.Usage = D3D11_USAGE_DEFAULT;

	UINT* init = new UINT[8];
	
	
	init[0] = 0;
	init[1] = 0;
	init[2] = 0;
	init[3] = 0;
	

	data.pSysMem = init;

	hr = this->gDevice->CreateBuffer(&nrBDesc, &data, &this->particleCountBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"particle count cbuffer creation failed", L"error", MB_OK);
	}

	//create indirect argument buffer used for the drawInstancedIndirect call
	D3D11_BUFFER_DESC inDesc;
	ZeroMemory(&inDesc, sizeof(D3D11_BUFFER_DESC));
	inDesc.ByteWidth = 4 * sizeof(UINT);
	inDesc.Usage = D3D11_USAGE_DEFAULT;
	inDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	init[1] = 1;
	hr = this->gDevice->CreateBuffer(&inDesc, &data, &this->IndirectArgsBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"inirect args buffer creation failed", L"error", MB_OK);
	}


	//create stageing buffer used for copying data from gpu to cpu
	/*D3D11_BUFFER_DESC stageDesc;
	ZeroMemory(&inDesc, sizeof(D3D11_BUFFER_DESC));
	stageDesc.ByteWidth = 8 * sizeof(UINT);
	stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stageDesc.Usage = D3D11_USAGE_STAGING;
	

	for (size_t i = 0; i < 8; i++)
	{
		init[i] = 0;
	}
	hr = this->gDevice->CreateBuffer(&stageDesc, &data, &this->StagingBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"STAGING BUFFER CREATION FAILED!", L"error", MB_OK);
	}*/
}

void GraphicsHandler::createVertexBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));


	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (UINT)(this->objInfo.vInfo.size() * sizeof(vertexInfo));

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->objInfo.vInfo.data();

	ZeroMemory(&this->defferedVertexBuffer, sizeof(ID3D11Buffer));

	HRESULT hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->defferedVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"vertex buffer creation failed", L"error", MB_OK);
	}


	UINT32 vertexSize = sizeof(vertexInfo);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->defferedVertexBuffer, &vertexSize, &offset);

}

void GraphicsHandler::createDepthBuffer()
{
	D3D11_TEXTURE2D_DESC dDesc;
	dDesc.Width = this->width;
	dDesc.Height = this->height;
	dDesc.MipLevels = 1;
	dDesc.ArraySize = 1;
	dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dDesc.SampleDesc.Count = 1;
	dDesc.SampleDesc.Quality = 0;
	dDesc.Usage = D3D11_USAGE_DEFAULT;
	dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dDesc.CPUAccessFlags = 0;
	dDesc.MiscFlags = 0;
	HRESULT hr = this->gDevice->CreateTexture2D(&dDesc, NULL, &this->depthBuffer);
	if (FAILED(hr))
		MessageBox(0, L"depth stencil resource creation failed", L"error", MB_OK);

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = this->gDevice->CreateDepthStencilState(&dsDesc, &this->dsState);
	if (FAILED(hr))
	{
		MessageBox(0, L"depth state creation failed", L"error", MB_OK);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvDesc.Format = dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;

	hr = gDevice->CreateDepthStencilView(this->depthBuffer, &dsvDesc, &this->DSV);
	if (FAILED(hr))
		MessageBox(0, L"depth stencil view creation failed", L"error", MB_OK);


	D3D11_DEPTH_STENCIL_DESC disabledDepthDesc;
	ZeroMemory(&disabledDepthDesc, sizeof(disabledDepthDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	disabledDepthDesc.DepthEnable = false;
	disabledDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	disabledDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	disabledDepthDesc.StencilEnable = true;
	disabledDepthDesc.StencilReadMask = 0xFF;
	disabledDepthDesc.StencilWriteMask = 0xFF;

	disabledDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	disabledDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	disabledDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	disabledDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	hr = gDevice->CreateDepthStencilState(&disabledDepthDesc, &this->disableDepthState);
	if (FAILED(hr))
		MessageBox(0, L"stensil state failed!", L"error", MB_OK);


}

void GraphicsHandler::createSamplers()
{
	ID3D11SamplerState* sState = nullptr;
	D3D11_SAMPLER_DESC sDesc;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sDesc.MaxAnisotropy = 1;
	sDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sDesc.MinLOD = 0;
	sDesc.MipLODBias = 0;

	this->gDevice->CreateSamplerState(&sDesc, &sState);
	this->gDeviceContext->PSSetSamplers(0, 1, &sState);

}

void GraphicsHandler::createLightBuffer()
{
	this->light.lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->light.lightPos = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	this->light.lightAngle = DirectX::XMFLOAT2(1.0f, 1.0f);
	this->light.lightDir = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->light.lightRange = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	DirectX::XMVECTOR temp;

	temp = DirectX::XMLoadFloat4(&this->light.lightPos);

	//temp = DirectX::XMVector4Transform(temp, this->cameraClass->getMatrix().world);

	DirectX::XMStoreFloat4(&this->light.lightPos, temp);


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(lightStruct);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &this->light;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->lightbuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"light buffer creation failed!", L"error", MB_OK);
	}

}

void GraphicsHandler::createDefferedBuffers()
{
	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Height = this->height;
	desc.Width = this->width;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = 1;


	HRESULT hr;

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		hr = this->gDevice->CreateTexture2D(&desc, NULL, &this->renderTargets[i]);
		if (FAILED(hr))
			MessageBox(0, L"Render target failed!", L"error", MB_OK);

		hr = this->gDevice->CreateRenderTargetView(this->renderTargets[i], &rtvDesc, &this->renderTargetViews[i]);
		if (FAILED(hr))
			MessageBox(0, L"Render target view failed!", L"error", MB_OK);

		hr = this->gDevice->CreateShaderResourceView(renderTargets[i], &srvDesc, &this->shaderResourceViews[i]);
		if (FAILED(hr))
			MessageBox(0, L"Shader resource view failed!", L"error", MB_OK);
	}

}

void GraphicsHandler::render()
{
	float clearColor[] = { 0, 0, 0, 1 };

	this->renderGeometry();


	//disable depth stencil
	this->gDeviceContext->OMSetDepthStencilState(this->disableDepthState, 1);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);

	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->PSSetShaderResources(0, NROFBUFFERS, this->shaderResourceViews);

	this->gDeviceContext->IASetInputLayout(this->vertexLayout);

	this->gDeviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->pixelShader, nullptr, 0);

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->lightbuffer);
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->mtlLightbuffer);

	this->gDeviceContext->OMSetRenderTargets(1, &this->rtvBackBuffer, this->DSV);

	//Clear depth stencil here
	this->gDeviceContext->ClearRenderTargetView(this->rtvBackBuffer, clearColor);
	this->gDeviceContext->ClearDepthStencilView(this->DSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	this->gDeviceContext->Draw(6, 0);

	this->gDeviceContext->OMSetDepthStencilState(this->dsState, 1);

	this->swapChain->Present(0, 0);
}

void GraphicsHandler::renderGeometry()
{
	//Kanske en specifik viwport for quadsaken
	float clearColor[] = { 0, 0, 0, 1 };

	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT32 vertexSize = sizeof(vertexInfo);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->defferedVertexBuffer, &vertexSize, &offset);

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
	

	this->gDeviceContext->PSSetShaderResources(0, 1, &this->textureView);

	this->gDeviceContext->IASetInputLayout(this->defferedVertexLayout);


	this->gDeviceContext->VSSetShader(this->defferedVertexShader, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->defferedPixelShader, nullptr, 0);

	//kanske ska vara andra tal, troligtvis inte
	this->setViewPort(480, 640);

	this->gDeviceContext->OMSetRenderTargets(NROFBUFFERS, this->renderTargetViews, this->DSV);

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->gDeviceContext->ClearRenderTargetView(this->renderTargetViews[i], clearColor);
	}

	//Kanskse borde vara drawindexed
	this->gDeviceContext->Draw(36, 0);

	//Reset
	this->gDeviceContext->OMSetDepthStencilState(this->dsState, 1);

	//Null stuff
	ID3D11RenderTargetView* temp[NROFBUFFERS];

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		temp[i] = NULL;
	}

	this->gDeviceContext->OMSetRenderTargets(NROFBUFFERS, temp, NULL);
}

void GraphicsHandler::renderParticles()
{
	float clearColor[] = { 0, 0, 0, 1 };

	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->setViewPort(this->height, this->width);
	this->gDeviceContext->VSSetShader(this->particleVertex, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->particleGeometry, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->particlePixel, nullptr, 0);

	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->matrixBuffer);
	this->gDeviceContext->CSSetShader(this->computeShader, nullptr, 0);

	this->gDeviceContext->OMSetRenderTargets(0, &this->rtvBackBuffer, this->DSV);

	this->gDeviceContext->ClearRenderTargetView(this->rtvBackBuffer, clearColor);
	this->gDeviceContext->ClearDepthStencilView(this->DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void GraphicsHandler::updateParticles()
{

}

#include "GraphicsHandler.h"


GraphicsHandler::GraphicsHandler(HWND wHandler, int height, int width)
{
	srand(time(NULL));
	this->currentTime = 0;
	this->deltaTime = 0;
	this->lastInsert = this->currentTime;
	this->lastFrame = 0;
	this->lastUpdate = 0;
	this->height = height;
	this->width = width;
	this->nrOfverticies = 0;


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
	this->particleCountBuffer = nullptr;
	this->IndirectArgsBuffer = nullptr;
	this->particleInserter = nullptr;
	this->deltaTimeBuffer = nullptr;

	this->particleGeometry = nullptr;
	this->particlePixel = nullptr;
	this->particleVertex = nullptr;

	this->defferedPixelShader = nullptr;
	this->defferedVertexShader = nullptr;
	this->dsState = nullptr;
	this->defferedVertexBuffer = nullptr;
	this->DSV = nullptr;
	this->mtlLightbuffer = nullptr;
	this->emitterlocation = nullptr;
	this->sState = nullptr;

	this->UAVS[0] = nullptr;
	this->UAVS[1] = nullptr;
	this->SRVS[0] = nullptr;
	this->SRVS[1] = nullptr;

	this->nullSRV = nullptr;
	this->nullUAV = nullptr;
	this->nullRTV = nullptr;
	this->nullDSV = nullptr;

	this->rState = nullptr;
	this->cameraPos = nullptr;
	this->debugDevice = nullptr;
	this->normalMapView = nullptr;
	this->airResistance = nullptr;
	

	this->instanceBuffer = nullptr;

	this->intancies = nullptr;
	this->terrainVS = nullptr;
	this->terrainLayout = nullptr;
	this->terraniShadowVertexShader = nullptr;

	this->visibleInstance = new Instance[INSTANCECOUNT];
	for (size_t i = 0; i < INSTANCECOUNT; i++)
	{
		this->visibleInstance[i].offset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			
	}
	this->visibleInstanceCount = 0;
	this->root = new BoxTree();

	

	

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->renderTargets[i] = nullptr;
		this->renderTargetViews[i] = nullptr;
		this->shaderResourceViews[i] = nullptr;
	}

	this->CreateDirect3DContext(wHandler);
	this->setViewPort(height, width);

	

	this->cameraClass = new CameraClass(this->gDevice, this->gDeviceContext, wHandler, width, height);
	this->terrainHandler = new TerrainHandler(
		this->gDevice,
		"../resource/maps/HeightMap4.bmp");

	this->createShaders();
	this->createTexture();
	this->createSamplers();
	this->loadMtl();
	this->loadObj();
	

	this->createTriangleData();
	this->createInstanceBuffer();

	this->createLightBuffer();
	this->createVertexBuffer();
	this->createMtlLightBuffer(MAXMTLS);
	//if you change nr of particles here remember to change in shaders too
	this->createParticleBuffers(512);
	this->particleFirstTimeInit();
	this->createRasterState();
	
	//Checking live objects
	//HRESULT hr = this->gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast <void **>(&debugDevice)); 
	//if (FAILED(hr))
	//{
	//	MessageBox(0, L"debug device creation failed", L"error", MB_OK);
	//}

	//Constant buffer till vertex shader
	this->matrixBuffer = this->cameraClass->createConstantBuffer();
	this->cameraPos = this->cameraClass->createCamrePosBuffer();
	this->createBoxTree(4);

	this->mFrustrum = new Frustrum();
	this->mFrustrum->constructFrustrum(this->cameraClass->getProjM(), this->cameraClass->getViewM());

	this->visibleTerrainVertices = new VertexInfo[terrainHandler->getNrOfVertices()];
	
	for (int i = 0; i < terrainHandler->getNrOfVertices(); i++)
	{
		this->visibleTerrainVertices[i] = {0};
	}
}

GraphicsHandler::~GraphicsHandler()
{

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
		D3D11_CREATE_DEVICE_DEBUG, //S�tt till null p� skoldatorrerneA D3D11_CREATE_DEVICE_DEBUG
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
		//Depth buffer borde nog h�nda h�r
		this->createDepthBuffers();
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

		//L�gg in depthviewsaken h�r i st�llet f�r nULL
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
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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
		{ "MTLNR", 0, DXGI_FORMAT_R32_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "OFFSET", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
		
	};

	hr = this->gDevice->CreateInputLayout(inputDescDeffered, ARRAYSIZE(inputDescDeffered), dvsBlob->GetBufferPointer(), dvsBlob->GetBufferSize(), &this->defferedVertexLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"input desc creation failed", L"error", MB_OK);
	}

	dvsBlob->Release();

	ID3DBlob* tvsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"TerrainVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&tvsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"dvsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(tvsBlob->GetBufferPointer(), tvsBlob->GetBufferSize(), NULL, &this->terrainVS);

	if (FAILED(hr))
	{
		MessageBox(0, L"terrain vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDescTerrain[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MTLNR", 0, DXGI_FORMAT_R32_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	hr = this->gDevice->CreateInputLayout(inputDescTerrain, ARRAYSIZE(inputDescTerrain), tvsBlob->GetBufferPointer(), tvsBlob->GetBufferSize(), &this->terrainLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"terraininput desc creation failed", L"error", MB_OK);
	}

	tvsBlob->Release();


	ID3DBlob* shadowVSBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ShadowVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shadowVSBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(shadowVSBlob->GetBufferPointer(), shadowVSBlob->GetBufferSize(), NULL, &this->shadowVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, L"shadow vertex shader creation failed", L"error", MB_OK);
	}

	ID3DBlob* terrainShadowVSBlob = nullptr;
	hr = D3DCompileFromFile(
		L"TerrainShadowVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&terrainShadowVSBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(terrainShadowVSBlob->GetBufferPointer(), terrainShadowVSBlob->GetBufferSize(), NULL, &this->terraniShadowVertexShader);

	if (FAILED(hr))
	{
		MessageBox(0, L"shadow vertex shader creation failed", L"error", MB_OK);
	}

	//Same input as deferred for easy managing

	terrainShadowVSBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"PixelShader.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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

	ID3D10Blob *arcsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"AirResistance.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&arcsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(arcsBlob->GetBufferPointer(), arcsBlob->GetBufferSize(), nullptr, &this->airResistance);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute Shader creation failed", L"error", MB_OK);
	}
	arcsBlob->Release();

	ID3D10Blob *icsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"ParticleInserter.hlsl",
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
	}

	pgsBlob->Release();

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


	ID3DBlob* gsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"GeometryShader.hlsl",
		NULL,
		NULL,
		"main",
		"gs_5_0",
		0,
		0,
		&gsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L" geometry shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &this->geometryShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"Geometry shader creation failed", L"error", MB_OK);
	}

	gsBlob->Release();


}

void GraphicsHandler::createTexture()
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../resource/Maps/kung.png", &this->textureResoure, &this->textureView);
	if (FAILED(hr))
	{
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}

	ID3D11Resource* texture = nullptr;
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../resource/Maps/normalMap.png", &texture, &this->normalMapView);
	if (FAILED(hr))
	{
		MessageBox(0, L"normalmap texture creation failed", L"error", MB_OK);
	}
	texture->Release();
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
		VertexInfo tempVInfo;


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

void GraphicsHandler::createMtlLightBuffer(const int MaxMTLS)
{
	//sets maximu nr of materials. shange in pixel shader to if you want more
	mtLight *ml = new mtLight[MaxMTLS]; 

	for (size_t i = 0; i < this->objInfo.nrOfMaterials; i++)
	{
		ml[i].ambient = this->objInfo.mInfo.at(i).ambient;
		ml[i].ambient.w = 0.0f;
		ml[i].diffuse = this->objInfo.mInfo.at(i).diffuse;
		ml[i].diffuse.w = 0.0f;
		ml[i].specular = this->objInfo.mInfo.at(i).specular;
		ml[i].specular.w = this->objInfo.mInfo.at(i).specWeight;
	}

	for (size_t i = this->objInfo.nrOfMaterials; i < 10; i++)
	{
		ml[i].ambient = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ml[i].diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ml[i].specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			
	}


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = MaxMTLS * sizeof(mtLight);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = ml;
	

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->mtlLightbuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"mtl light buffer creation failed!", L"error", MB_OK);
	}

	delete[] ml;
}

void GraphicsHandler::createParticleBuffers(int nrOfPArticles)
{
	
	Particle *particles = new Particle[nrOfPArticles];
	EmitterLocation eLocation;
	eLocation.emitterLocation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	float x, y, z;
	x = rand() % 255;
	y = rand() % 255;
	z = rand() % 255;
	DirectX::XMVECTOR randVec = DirectX::XMVectorSet(x, y, z, 1.0f);
	randVec = DirectX::XMVector4Normalize(randVec);
	DirectX::XMFLOAT4 temp;
	DirectX::XMStoreFloat4(&temp, randVec);
	temp.w = 1.0f;
	eLocation.randomVector = temp;
	

	for (size_t i = 0; i < nrOfPArticles; i++)
	{
		particles[i].position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		particles[i].velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		particles[i].age = 0.0f;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(EmitterLocation);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &eLocation;

	//creates emitter location constant buffer
	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->emitterlocation);
	if (FAILED(hr))
	{
		MessageBox(0, L"emitter creation failed", L"error", MB_OK);
	}

	desc.ByteWidth = sizeof(float)* 4;
	data.pSysMem = &this->deltaTime;
	hr = this->gDevice->CreateBuffer(&desc, &data, &this->deltaTimeBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"deltaTimeBuffer creation failed", L"error", MB_OK);
	}

	data.pSysMem = particles;

	D3D11_BUFFER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(D3D11_BUFFER_DESC));

	sDesc.ByteWidth = nrOfPArticles * sizeof(Particle);
	sDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	sDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sDesc.StructureByteStride = sizeof(Particle);
	sDesc.Usage = D3D11_USAGE_DEFAULT;
	sDesc.CPUAccessFlags = 0;


	//creates 2 structured buffers who is going to be used as append/consume buffers for the particle system
	ID3D11Buffer* structBuffer1 = nullptr;
	ID3D11Buffer* structBuffer2 = nullptr;
	hr = this->gDevice->CreateBuffer(&sDesc, &data, &structBuffer1);
	if (FAILED(hr))
	{
		MessageBox(0, L"struct buffer 1 creation failed", L"error", MB_OK);
	}
	
	hr = this->gDevice->CreateBuffer(&sDesc, &data, &structBuffer2);
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


	//uav for updating buffer srv for viewing 
	//uav and srv for first buffer
	hr = this->gDevice->CreateUnorderedAccessView(structBuffer1, &uavDesc, &this->UAVS[0]);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 1 creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateShaderResourceView(structBuffer1, &srvDesc, &this->SRVS[0]);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 1 creation failed", L"error", MB_OK);
	}

	//uav and srv for second buffer
	hr = this->gDevice->CreateUnorderedAccessView(structBuffer2, &uavDesc, &this->UAVS[1]);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 2 creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateShaderResourceView(structBuffer2, &srvDesc, &this->SRVS[1]);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 2 creation failed", L"error", MB_OK);
	}
	structBuffer1->Release();
	structBuffer2->Release();


	//this is for the indirect args buffer. For  constant buffer only first value is relevant rest is padding.
	//0: nr of verticies
	//1: nr of instances
	//2: start vertex
	//3: start instance
	//4: padding
	UINT* init = new UINT[5];
	init[0] = 0;
	init[1] = 0;
	init[2] = 0;
	init[3] = 0;
	init[4] = 0;

	//create constant buffer who holds the nr of particles
	D3D11_BUFFER_DESC nrBDesc;
	ZeroMemory(&nrBDesc, sizeof(D3D11_BUFFER_DESC));
	nrBDesc.ByteWidth = 4 * sizeof(UINT);
	nrBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	nrBDesc.Usage = D3D11_USAGE_DEFAULT;


	data.pSysMem = init;

	hr = this->gDevice->CreateBuffer(&nrBDesc, &data, &this->particleCountBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"particle count cbuffer creation failed", L"error", MB_OK);
	}

	//create indirect argument buffer used for the drawIndirect call
	D3D11_BUFFER_DESC inDesc;
	ZeroMemory(&inDesc, sizeof(D3D11_BUFFER_DESC));
	inDesc.ByteWidth = 5 * sizeof(UINT);
	inDesc.Usage = D3D11_USAGE_DEFAULT;
	inDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	//sets nr of insatnces
	init[1] = 1;


	hr = this->gDevice->CreateBuffer(&inDesc, &data, &this->IndirectArgsBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"indirect args buffer creation failed", L"error", MB_OK);
	}

	delete[] init;
}

void GraphicsHandler::createRasterState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	HRESULT hr = this->gDevice->CreateRasterizerState(&desc, &this->rState);
	if (FAILED(hr))
	{
		MessageBox(0, L"raster state creation failed", L"error", MB_OK);
	}
	this->gDeviceContext->RSSetState(this->rState);
}

void GraphicsHandler::createVertexBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));


	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (UINT)(this->objInfo.vInfo.size() * sizeof(VertexInfo));

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = this->objInfo.vInfo.data();

	ZeroMemory(&this->defferedVertexBuffer, sizeof(ID3D11Buffer));

	HRESULT hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->defferedVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"vertex buffer creation failed", L"error", MB_OK);
	}
}

void GraphicsHandler::createDepthBuffers()
{
	D3D11_TEXTURE2D_DESC dDesc;
	dDesc.Width = this->width;
	dDesc.Height = this->height;
	dDesc.MipLevels = 1;
	dDesc.ArraySize = 1;
	dDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dDesc.SampleDesc.Count = 1;
	dDesc.SampleDesc.Quality = 0;
	dDesc.Usage = D3D11_USAGE_DEFAULT;
	dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
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

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
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
	disabledDepthDesc = dsDesc;
	disabledDepthDesc.DepthEnable = false;

	// Create the state using the device.
	hr = gDevice->CreateDepthStencilState(&disabledDepthDesc, &this->disableDepthState);
	if (FAILED(hr))
		MessageBox(0, L"stensil state failed!", L"error", MB_OK);

	//Create depth buffer for shadow map creation
	D3D11_TEXTURE2D_DESC shadowTexDesc;
	shadowTexDesc.Width = this->width;
	shadowTexDesc.Height = this->height;
	shadowTexDesc.MipLevels = 1;
	shadowTexDesc.ArraySize = 1;
	shadowTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowTexDesc.SampleDesc.Count = 1;
	shadowTexDesc.SampleDesc.Quality = 0;
	shadowTexDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowTexDesc.CPUAccessFlags = 0;
	shadowTexDesc.MiscFlags = 0;
	

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowdsvDesc;
	ZeroMemory(&shadowdsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	shadowdsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	shadowdsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowdsvDesc.Texture2D.MipSlice = 0;
	shadowdsvDesc.Flags = 0;
	

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = shadowTexDesc.MipLevels;

	hr = this->gDevice->CreateTexture2D(&shadowTexDesc, NULL, &this->shadowDepthBuffer);
	if (FAILED(hr))
		MessageBox(0, L"shadowDepthBuffer state failed!", L"error", MB_OK);

	hr = this->gDevice->CreateDepthStencilView(this->shadowDepthBuffer, &shadowdsvDesc, &this->shadowDSV);
	if (FAILED(hr))
		MessageBox(0, L"shadowdsvDesc state failed!", L"error", MB_OK);

	hr = this->gDevice->CreateShaderResourceView(this->shadowDepthBuffer, &srvDesc, &this->shadowSRV);
	if (FAILED(hr))
		MessageBox(0, L"srvDesc state failed!", L"error", MB_OK);
}

void GraphicsHandler::createSamplers()
{
	D3D11_SAMPLER_DESC sDesc;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.BorderColor[0] = 1;
	sDesc.BorderColor[1] = 1;
	sDesc.BorderColor[2] = 1;
	sDesc.BorderColor[3] = 1;

	sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.MaxAnisotropy = 1;
	sDesc.MaxLOD = 0;
	sDesc.MinLOD = 0;
	sDesc.MipLODBias = 0;

	this->gDevice->CreateSamplerState(&sDesc, &sState);

}

void GraphicsHandler::createLightBuffer()
{
	this->light.lightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	this->light.lightPos = DirectX::XMFLOAT4(0.0f, 13.0f, 0.f, 1.0f);
	this->light.lightAngle = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
	this->light.lightDir = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	this->light.lightRange = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

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
		MessageBox(0, L"light buffer creation failed!", L"error", MB_OK);

	this->createLightMatrices();

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
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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

	//disable depth stencil. Anledningen till det �r f�r att vi nu renderar i 2D p� en stor quad, det finns inget djup l�ngre
	this->gDeviceContext->OMSetDepthStencilState(this->disableDepthState, 1);
	this->gDeviceContext->OMSetRenderTargets(1, &this->rtvBackBuffer, this->DSV);

	//Clear depth stencil here
	this->gDeviceContext->ClearRenderTargetView(this->rtvBackBuffer, clearColor);
	this->gDeviceContext->ClearDepthStencilView(this->DSV, D3D11_CLEAR_DEPTH, 1.f, 0);


	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);

	this->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->PSSetShaderResources(0, NROFBUFFERS, this->shaderResourceViews);

	//setting shadow map
	this->gDeviceContext->PSSetShaderResources(4, 1, &this->shadowSRV);
	

	this->gDeviceContext->IASetInputLayout(this->vertexLayout);

	this->gDeviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->pixelShader, nullptr, 0);

	this->gDeviceContext->PSSetSamplers(0, 1, &this->sState);

	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);

	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->lightbuffer);
	this->gDeviceContext->PSSetConstantBuffers(1, 1, &this->cameraPos);
	this->gDeviceContext->PSSetConstantBuffers(2, 1, &this->mtlLightbuffer);
	this->gDeviceContext->PSSetConstantBuffers(3, 1, &this->lightMatrixBuffer);
	
	this->gDeviceContext->OMSetRenderTargets(1, &this->rtvBackBuffer, this->DSV);

	
	this->gDeviceContext->ClearRenderTargetView(this->rtvBackBuffer, clearColor);

	this->gDeviceContext->Draw(6, 0);
	//Clear depth stencil here
	this->gDeviceContext->ClearDepthStencilView(this->DSV, D3D11_CLEAR_DEPTH, 1.f, 0);
	this->gDeviceContext->OMSetDepthStencilState(this->dsState, 1);
	

	this->swapChain->Present(0, 0);

	//Nulling
	for (size_t i = 0; i < NROFBUFFERS; i++)
	{
		this->gDeviceContext->PSSetShaderResources(i, 1, &this->nullSRV);
	}

	//Deth�r vart inte s� fint
	this->gDeviceContext->PSSetShaderResources(4, 1, &this->nullSRV);
}

void GraphicsHandler::renderShadows()
{

	this->gDeviceContext->OMSetRenderTargets(0, nullptr, this->shadowDSV);
	this->gDeviceContext->ClearDepthStencilView(this->shadowDSV, D3D11_CLEAR_DEPTH, 1.f, 0);


	this->gDeviceContext->VSSetShader(this->terraniShadowVertexShader, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	this->gDeviceContext->IASetInputLayout(this->terrainLayout);


	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->lightMatrixBuffer);

	this->terrainHandler->setShaderResources(this->gDeviceContext);
	this->terrainHandler->renderTerrain(this->gDeviceContext);

	this->gDeviceContext->VSSetShader(this->shadowVertexShader, nullptr, 0);
	this->gDeviceContext->IASetInputLayout(this->defferedVertexLayout);
	UINT32 vertexSize = sizeof(VertexInfo);
	UINT32 intanceSize = sizeof(Instance);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->defferedVertexBuffer, &vertexSize, &offset);
	this->gDeviceContext->IASetVertexBuffers(1, 1, &this->instanceBuffer, &intanceSize, &offset);

	this->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->gDeviceContext->DrawInstanced(36, this->visibleInstanceCount, 0, 0);

	//Nulling
	ID3D11DepthStencilView* nullshadowDSV = nullptr;
	this->gDeviceContext->OMSetRenderTargets(0, nullptr, nullshadowDSV);

}

void GraphicsHandler::renderGeometry()
{
	float clearColor[] = { 102/255.0f, 152/255.0f, 255/255.0f, 1 };
	this->gDeviceContext->OMSetRenderTargets(NROFBUFFERS, this->renderTargetViews, this->DSV);

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->gDeviceContext->ClearRenderTargetView(this->renderTargetViews[i], clearColor);
	}

	//Set deffered shaders and resources
	this->gDeviceContext->VSSetShader(this->terrainVS, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->geometryShader, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->defferedPixelShader, nullptr, 0);

	this->gDeviceContext->IASetInputLayout(this->terrainLayout);
	

	
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->cameraPos);
	this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->mtlLightbuffer);
	this->gDeviceContext->PSSetShaderResources(0, 1, &this->textureView);
	this->gDeviceContext->PSSetShaderResources(1, 1, &this->normalMapView);

	
	this->setViewPort(this->height, this->width);

	this->gDeviceContext->PSSetSamplers(0, 1, &this->sState);

	//Draw terrain
	this->terrainHandler->setShaderResources(this->gDeviceContext);
	this->terrainHandler->renderTerrain(this->gDeviceContext);

	//Changing the vertex shader
	this->gDeviceContext->VSSetShader(this->defferedVertexShader, nullptr, 0);
	this->gDeviceContext->IASetInputLayout(this->defferedVertexLayout);
	this->gDeviceContext->OMSetDepthStencilState(this->dsState, 1);
	
	//Draw objects
	UINT32 vertexSize = sizeof(VertexInfo);
	UINT32 offset = 0;
	UINT32 intanceSize = sizeof(Instance);
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->defferedVertexBuffer, &vertexSize, &offset);
	this->gDeviceContext->IASetVertexBuffers(1, 1, &this->instanceBuffer, &intanceSize, &offset);
	this->gDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (this->visibleInstanceCount != 0)
	{
		this->gDeviceContext->DrawInstanced(36, this->visibleInstanceCount, 0, 0);
	}
	


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
	
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->setViewPort(this->height, this->width);
	this->gDeviceContext->VSSetShader(this->particleVertex, nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->particleGeometry, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->particlePixel, nullptr, 0);
	this->gDeviceContext->IASetInputLayout(nullptr);
	this->gDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	
	this->gDeviceContext->VSSetShaderResources(0, 0, nullptr);
	this->gDeviceContext->VSSetShaderResources(0, 1, &this->SRVS[0]);
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &this->matrixBuffer);
	this->gDeviceContext->PSSetShaderResources(0, 1, &this->textureView);


	this->gDeviceContext->OMSetRenderTargets(NROFBUFFERS, this->renderTargetViews, this->DSV);

	this->gDeviceContext->OMSetDepthStencilState(this->dsState, 1);

	this->gDeviceContext->DrawInstancedIndirect(this->IndirectArgsBuffer, 0);

	//clear stuff for later stages
	this->gDeviceContext->VSSetShaderResources(0, 1, &this->nullSRV);
	ID3D11RenderTargetView* temp[NROFBUFFERS];
	for (int i = 0; i < NROFBUFFERS; i++)
	{
		temp[i] = NULL;
	}
	this->gDeviceContext->OMSetRenderTargets(NROFBUFFERS, temp, NULL);
}

void GraphicsHandler::update(float deltaT)
{
	this->deltaTime = deltaT;
	this->currentTime += deltaT;
	this->cameraClass->update(deltaT);
	this->cameraClass->updatecameraPosBuffer(this->cameraPos);
	this->cameraClass->updateConstantBuffer(this->matrixBuffer);
	this->terrainHandler->walkOnTerrain(this->cameraClass->getCameraPos());
	this->updateLightBuffer();

	if (this->currentTime - this->lastUpdate >= 0.8f)
	{
		this->lastUpdate = this->currentTime;
		this->updateParticleCBuffers();
		this->updateParticles();
	}
	if (this->cameraClass->airResistance())
	{
		this->updateFrustrum();
	}

	//locks fps at 60
	if (this->currentTime - this->lastFrame >= 16.0f)
	{
		this->lastFrame = this->currentTime;
		
		this->cull();
		
		this->renderShadows();
		this->renderGeometry();
		this->renderParticles();
		this->render();
	}
}

void GraphicsHandler::updateParticles()
{

	//creates new particles every 5th second
	if (this->currentTime - this->lastInsert > 500.0f)
	{
		this->lastInsert = currentTime;
		this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->emitterlocation);
		this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);
		this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->particleCountBuffer);

		this->gDeviceContext->CSSetShader(this->particleInserter, nullptr, 0);
		this->gDeviceContext->Dispatch(1, 1, 1);
		this->gDeviceContext->CopyStructureCount(this->particleCountBuffer, 0, this->UAVS[0]);

		this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &UAVFLAG);
		this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &UAVFLAG);
	}
	

	
	this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->particleCountBuffer);
	this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->deltaTimeBuffer);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->UAVS[1], &startParticleCount);

	

	this->gDeviceContext->CSSetShader(this->airResistance, nullptr, 0);

	
	


	this->gDeviceContext->Dispatch(1, 1, 1);
	
	this->swapParticleBuffers();


	this->gDeviceContext->CopyStructureCount(this->particleCountBuffer, 0, this->UAVS[0]);
	this->gDeviceContext->CopyStructureCount(this->IndirectArgsBuffer, 0, this->UAVS[0]);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &UAVFLAG);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &UAVFLAG);

}

void GraphicsHandler::swapParticleBuffers()
{
	ID3D11UnorderedAccessView *tempUAV;
	ID3D11ShaderResourceView *tempSRV;
	tempUAV = this->UAVS[0];
	tempSRV = this->SRVS[0];

	this->UAVS[0] = this->UAVS[1];
	this->SRVS[0] = this->SRVS[1];

	this->UAVS[1] = tempUAV;
	this->SRVS[1] = tempSRV;

	tempSRV = nullptr;
	tempUAV = nullptr;

}

void GraphicsHandler::particleFirstTimeInit()
{
	this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->emitterlocation);
	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &startParticleCount);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->UAVS[1], &startParticleCount);
	this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->particleCountBuffer);

	this->gDeviceContext->CSSetShader(this->particleInserter, nullptr, 0);



	this->gDeviceContext->Dispatch(1, 1, 1);
	this->gDeviceContext->CopyStructureCount(this->particleCountBuffer, 0, this->UAVS[0]);	
	this->lastInsert = this->currentTime;

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &startParticleCount);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &startParticleCount);
}

void GraphicsHandler::updateParticleCBuffers()
{
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->emitterlocation, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	EmitterLocation eLocation;
	eLocation.emitterLocation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 temp;

	float x, y, z;
	x = rand();
	y = rand();
	z = rand();
	DirectX::XMVECTOR randVec = DirectX::XMVectorSet(x, y, z, 1.0f);

	randVec = DirectX::XMVector4Normalize(randVec);

	DirectX::XMStoreFloat4(&temp, randVec);
	temp.w = 1.0f;
	eLocation.randomVector = temp;

	memcpy(data.pData, &eLocation, sizeof(eLocation));

	this->gDeviceContext->Unmap(this->emitterlocation, 0);

	this->gDeviceContext->Map(this->deltaTimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);


	

	memcpy(data.pData, &this->deltaTime, sizeof(this->deltaTime));

	this->gDeviceContext->Unmap(this->deltaTimeBuffer, 0);
}

void GraphicsHandler::kill()
{
	ULONG test = 0;

	this->gDeviceContext->ClearState();


	test = this->swapChain->Release();
	test = this->rtvBackBuffer->Release();


	test = this->vertexShader->Release();
	test = this->defferedVertexShader->Release();
	test = this->shadowVertexShader->Release();
	test = this->pixelShader->Release();
	test = this->defferedPixelShader->Release();
	test = this->geometryShader->Release();
	test = this->vertexLayout->Release();
	test = this->defferedVertexLayout->Release();
	test = this->vertexBuffer->Release();
	test = this->defferedVertexBuffer->Release();
	test = this->matrixBuffer->Release();
	test = this->lightbuffer->Release();
	test = this->mtlLightbuffer->Release();
	test = this->lightMatrixBuffer->Release();
	test = this->sState->Release();
	test = this->textureResoure->Release();
	test = this->textureView->Release();

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		test = this->renderTargets[i]->Release();
		test = this->renderTargetViews[i]->Release();
		test = this->shaderResourceViews[i]->Release();
	}

	test = this->disableDepthState->Release();
	test = this->depthBuffer->Release();
	test = this->dsState->Release();
	test = this->DSV->Release();
	test = this->shadowDepthBuffer->Release();
	test = this->shadowDSV->Release();
	test = this->shadowSRV->Release();

	this->terrainHandler->kill();

	delete this->cameraClass;
	delete this->terrainHandler;

	test = this->particlePixel->Release();
	test = this->particleVertex->Release();
	test = this->particleGeometry->Release();
	test = this->particleInserter->Release();
	test = this->emitterlocation->Release();
	test = this->particleCountBuffer->Release();
	test = this->IndirectArgsBuffer->Release();
	test = this->deltaTimeBuffer->Release();
	test = this->UAVS[0]->Release();
	test = this->UAVS[1]->Release();
	test = this->SRVS[0]->Release();
	test = this->SRVS[1]->Release();
	test = this->airResistance->Release();
	test = this->rState->Release();
	test = this->cameraPos->Release();
	test = this->normalMapView->Release();
	test = this->terrainVS->Release();
	test = this->terrainLayout->Release();
	test = this->instanceBuffer->Release();
	test = this->terraniShadowVertexShader->Release();





	test = this->gDeviceContext->Release();
	test = this->gDevice->Release();
	delete[] this->intancies;
	delete[] this->visibleInstance;
	delete this->root;
	delete[] this->visibleTerrainVertices;
	delete this->mFrustrum;


	/*this->debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	this->debugDevice->Release();*/
}

void GraphicsHandler::createInstanceBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	this->intancies = new Instance[INSTANCECOUNT];

	this->intancies[0].offset = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	this->intancies[1].offset = DirectX::XMFLOAT3(-5.1f, 0.1f, 10.1f);
	this->intancies[2].offset = DirectX::XMFLOAT3(10.1f, 0.1f, 5.1f);
	this->intancies[3].offset = DirectX::XMFLOAT3(-5.1f, 0.1f, -5.1f);
	this->intancies[4].offset = DirectX::XMFLOAT3(0.1f, 5.1f, 4.1f);
	this->intancies[5].offset = DirectX::XMFLOAT3(-10.0f, 5.1f, -5.1f);
	this->intancies[6].offset = DirectX::XMFLOAT3(5.1f, 5.1f, 5.1f);
	this->intancies[7].offset = DirectX::XMFLOAT3(0.1f, 5.1f, -5.1f);

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Instance) * INSTANCECOUNT;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = this->intancies;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->instanceBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"instance buffer creation failed", L"error", MB_OK);
	}

}


void GraphicsHandler::createLightMatrices()
{
	DirectX::XMVECTOR eyePosition;
	eyePosition = DirectX::XMVectorSet(0, 13, 1, 0);

	DirectX::XMVECTOR focusPosition;
	focusPosition = DirectX::XMVectorSet(0, 0, 0, 0);

	DirectX::XMVECTOR upDirection;
	upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

	DirectX::XMMATRIX vLight = DirectX::XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
	vLight = DirectX::XMMatrixTranspose(vLight);

	DirectX::XMMATRIX pLight = DirectX::XMMatrixOrthographicLH(this->width/10.f, this->height/10.f, 0.1f, 200);
	pLight = DirectX::XMMatrixTranspose(pLight);

	this->lightMatrices.projection = pLight;
	this->lightMatrices.view = vLight;
	this->lightMatrices.world = this->cameraClass->getMatrix().world;


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(matrixStruct);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &lightMatrices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->lightMatrixBuffer);
	if (FAILED(hr))
		MessageBox(0, L"shadow buffer failed!", L"error", MB_OK);
}

void GraphicsHandler::updateLightBuffer()
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	//Shadow buffer
	this->gDeviceContext->Map(this->lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	this->lightMatrices.world = this->cameraClass->getMatrix().world;
	
	memcpy(dataPtr.pData, &this->lightMatrices, sizeof(matrixStruct));

	this->gDeviceContext->Unmap(lightMatrixBuffer, 0);

	//Light buffer
	this->gDeviceContext->Map(this->lightbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	DirectX::XMVECTOR temp = DirectX::XMLoadFloat4(&this->light.lightPos);
	temp = DirectX::XMVector4Transform(temp, this->cameraClass->getMatrix().world);
	DirectX::XMStoreFloat4(&this->light.lightPos, temp);

	memcpy(dataPtr.pData, &this->light, sizeof(matrixStruct));

	this->gDeviceContext->Unmap(lightbuffer, 0);
}

void GraphicsHandler::createBoxTree(int nrOfSplits)
{
	AABB aabb;
	aabb.p0 = DirectX::XMFLOAT3(-15.0f, -15.0f, -15.0f);
	aabb.p1 = DirectX::XMFLOAT3(15.0f, 15.0f, 15.0f);
	std::vector<Instance> temp;
	for (size_t i = 0; i < INSTANCECOUNT; i++)
	{
		temp.push_back(this->intancies[i]);
	}

	this->_createBoxTree(nrOfSplits, aabb, this->root, temp, INSTANCECOUNT);
}

void GraphicsHandler::_createBoxTree(int nrOfSplits, AABB aabb, BoxTree *&branch, std::vector<Instance> data, int instanceCount)
{
	branch = new BoxTree();
	branch->boundingVolume = aabb;
	branch->data = data;
	branch->instanceCount = instanceCount;

	if (nrOfSplits >= 0)
	{
		nrOfSplits--;
		

		//creates four new smaller aabbs for the four branches

		//downLeft
		AABB downL;
		downL.p0 = aabb.p0;
		downL.p1.x = (aabb.p0.x + aabb.p1.x) / 2;
		downL.p1.y = (aabb.p0.y + aabb.p1.y) / 2;
		downL.p1.z = aabb.p1.z;

		//downRight
		AABB downR;
		downR.p0.x = (aabb.p0.x + aabb.p1.x) / 2;
		downR.p0.y = aabb.p0.y;
		downR.p0.z = aabb.p0.z;
		downR.p1.x = aabb.p1.x;
		downR.p1.y = (aabb.p0.y +aabb.p1.y) / 2;
		downR.p1.z = aabb.p1.z;

		
		//upLeft
		AABB upL;
		upL.p0.x = aabb.p0.x;
		upL.p0.y = (aabb.p0.y + aabb.p1.y) / 2;
		upL.p0.z = aabb.p0.z;
		upL.p1.x = (aabb.p0.x + aabb.p1.x) / 2;
		upL.p1.y = aabb.p1.y;
		upL.p1.z = aabb.p1.z;

		//upRight
		AABB upR;
		upR.p0.x = (aabb.p0.x + aabb.p1.x) / 2;
		upR.p0.y = (aabb.p0.y + aabb.p1.y) / 2;
		upR.p0.z = aabb.p0.z;
		upR.p1 = aabb.p1;

		int dld = 0;
		std::vector<Instance> downLeftData;

		int uld = 0;
		std::vector<Instance> upLeftData;

		int drd = 0;
		std::vector<Instance> downRightData;

		int urd = 0;
		std::vector<Instance> upRightData;

		for (size_t i = 0; i < instanceCount; i++)
		{
			if (pointVSAABB(data[i].offset, downL))
			{
				downLeftData.push_back(data[i]);
				dld++;
			}
			if (pointVSAABB(data[i].offset, downR))
			{
				downRightData.push_back(data[i]);
				drd++;
			}
			if (pointVSAABB(data[i].offset, upL))
			{
				upLeftData.push_back(data[i]);
				uld++;
			}
			if (pointVSAABB(data[i].offset, upR))
			{
				upRightData.push_back(data[i]);
				urd++;
			}
		}

		{
			this->_createBoxTree(nrOfSplits, downL, branch->downLeft, downLeftData, dld);
		}

		{
			this->_createBoxTree(nrOfSplits, upL, branch->upLeft, upLeftData, uld);
		}

		{
			this->_createBoxTree(nrOfSplits, downR, branch->downRight, downRightData, drd);
		}

		{
			this->_createBoxTree(nrOfSplits, upR, branch->upRight, upRightData, urd);
		}	
	}
}

bool GraphicsHandler::pointVSAABB(DirectX::XMFLOAT3 point, AABB box)
{
	//checks if point is within aabb
	bool inside = true;
	if (point.x < box.p0.x)
	{
		inside = false;
	}
	if (point.y < box.p0.y)
	{
		inside = false;
	}
	if (point.z < box.p0.z)
	{
		inside = false;
	}
	if (point.x > box.p1.x)
	{
		inside = false;
	}
	if (point.y > box.p1.y)
	{
		inside = false;
	}
	if (point.z > box.p1.z)
	{
		inside = false;
	}


	return inside;


}

void GraphicsHandler::updateFrustrum()
{
	
	this->mFrustrum->constructFrustrum(this->cameraClass->getProjM(), this->cameraClass->getViewM());
}

void GraphicsHandler::cull()
{	
	//culls the cubes
	this->cullBoxes();
	
	//Not very performace efficient
	this->cullGeometry();
}

void GraphicsHandler::cullGeometry()
{
	//culls the terrain
	if (mFrustrum->AABBVsFrustrum(terrainHandler->GetFrustumTree()->boundingVolume))
	{
		this->terrainVerticeAmount = 0;
		traverseTerrainTree(terrainHandler->GetFrustumTree());

	}

	this->terrainHandler->updateVertexBuffer(this->gDeviceContext, this->visibleTerrainVertices, this->terrainVerticeAmount);
}

void GraphicsHandler::cullBoxes()
{
	this->visibleInstanceCount = 0;
	if (this->mFrustrum->AABBVsFrustrum(this->root->boundingVolume))
	{
		this->traverseBoxTree(this->root);
	}

	//this->test();

	if (this->visibleInstanceCount > 0)
	{
		D3D11_MAPPED_SUBRESOURCE data;
		ZeroMemory(&data, sizeof(D3D11_MAPPED_SUBRESOURCE));

		this->gDeviceContext->Map(this->instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy(data.pData, this->visibleInstance, sizeof(Instance)*this->visibleInstanceCount);
		this->gDeviceContext->Unmap(this->instanceBuffer, 0);
	}
	
}

void GraphicsHandler::traverseBoxTree(BoxTree* branch)
{
	if (branch->instanceCount != 0)
	{
		//if leaf is found
		if (branch->downLeft == nullptr && branch->downRight == nullptr && branch->upLeft == nullptr && branch->upRight == nullptr)
		{
			int temp = this->visibleInstanceCount;
			for (size_t i = 0; i < branch->instanceCount; i++)
			{
				this->visibleInstance[temp + i] = branch->data[i];
			}
			this->visibleInstanceCount += branch->instanceCount;
		}
		else
		{
			//traverse down tree if bounding volume is within frustrum
			if ( branch->downLeft != nullptr && this->mFrustrum->AABBVsFrustrum(branch->downLeft->boundingVolume))
			{
				this->traverseBoxTree(branch->downLeft);
			}
			if (branch->upLeft != nullptr && this->mFrustrum->AABBVsFrustrum(branch->upLeft->boundingVolume))
			{
				this->traverseBoxTree(branch->upLeft);
			}
			if (branch->downRight != nullptr && this->mFrustrum->AABBVsFrustrum(branch->downRight->boundingVolume))
			{
				this->traverseBoxTree(branch->downRight);
			}
			if (branch->upRight != nullptr && this->mFrustrum->AABBVsFrustrum(branch->upRight->boundingVolume))
			{
				this->traverseBoxTree(branch->upRight);
			}

		}
	}
}


void GraphicsHandler::traverseTerrainTree(FrustumTree* branch)
{
	if (branch->NE != nullptr)
	{
		if (mFrustrum->AABBVsFrustrum(branch->NE->boundingVolume))
			traverseTerrainTree(branch->NE);

		if (mFrustrum->AABBVsFrustrum(branch->NW->boundingVolume))
			traverseTerrainTree(branch->NW);

		if (mFrustrum->AABBVsFrustrum(branch->SW->boundingVolume))
			traverseTerrainTree(branch->SW);

		if (mFrustrum->AABBVsFrustrum(branch->SE->boundingVolume))
			traverseTerrainTree(branch->SE);
	}

	else
	{
		memcpy(&this->visibleTerrainVertices[this->terrainVerticeAmount], branch->data, sizeof(VertexInfo) * branch->vertexCount);

		this->terrainVerticeAmount += branch->vertexCount;
	}
}

void GraphicsHandler::test()
{
	this->visibleInstanceCount = 0;
	for (size_t i = 0; i < INSTANCECOUNT; i++)
	{
		AABB temp;
		temp.p0 = DirectX::XMFLOAT3(this->intancies[i].offset.x - 0.5f, this->intancies[i].offset.y - 0.5f, this->intancies[i].offset.z - 0.5f);
		temp.p1 = DirectX::XMFLOAT3(this->intancies[i].offset.x + 0.5f, this->intancies[i].offset.y + 0.5f, this->intancies[i].offset.z + 0.5f);
		if (this->mFrustrum->AABBVsFrustrum(temp))
		{
			this->visibleInstance[this->visibleInstanceCount] = this->intancies[i];
			this->visibleInstanceCount++;
		}
	}

}



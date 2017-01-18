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

	for (int i = 0; i < NROFBUFFERS; i++)
	{
		this->renderTargets[i] = nullptr;
		this->renderTargetViews[i] = nullptr;
		this->shaderResourceViews[i] = nullptr;
	}

	this->CreateDirect3DContext(wHandler);
	this->setViewPort(height, width);
	this->createShaders();
	//this->createTriangleData();
	this->createTexture();
	this->objInfo = this->loadObj();

	createVertexBuffer();
	



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
		this->createDefferedBuffers();


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

		this->cameraClass = new CameraClass(this->gDevice);
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

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->defferedPixelShader);
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
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"Deffered psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"Deffered pixel shader creation failed", L"error", MB_OK);
	}

	dpsBlob->Release();

}

void GraphicsHandler::createTexture()
{
	DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../resource/Maps/kung.png", &this->textureResoure, &this->textureView);

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


	this->matrixBuffer = cameraClass->createConstantBuffer();
	this->gDeviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
}

objectInfo GraphicsHandler::loadObj()
{
	std::string fileName("../resource/Cube.obj"), identifier;
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

			//uv 1 in face i
			tempVInfo.u = uv.at(f.at(i).b1 - 1).u;
			tempVInfo.v = uv.at(f.at(i).b1 - 1).v;

			//normal 1 in face i 
			tempVInfo.vnx = vn.at(f.at(i).c1 - 1).x;
			tempVInfo.vny = vn.at(f.at(i).c1 - 1).y;
			tempVInfo.vnz = vn.at(f.at(i).c1 - 1).z;
			

		
		

			objInfo.vInfo.push_back(tempVInfo);

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
			
		
				
			

			objInfo.vInfo.push_back(tempVInfo);

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
			
			

			objInfo.vInfo.push_back(tempVInfo);

		}

		objInfo.nrOfVertices = (int)(vp.size());
		objInfo.norOfIndexcies = (int)(f.size());

		objInfo.iInfo = f;
	}
	return objInfo;
}

void GraphicsHandler::loadMtl()
{
	std::string fileName("../resource/Cube.mtl"), identifier;
	std::string line;
	std::ifstream file(fileName);
	mtlInfo tempMInfo;
	bool cont = true;

	if (file.is_open() == false)
	{
		MessageBox(0, L"obj file failed to open", L"error", MB_OK);
	}
	else
	{
		std::istringstream inputString;
		

		while (std::getline(file, line))
		{
			inputString.str(line);

			if (line.substr(0, 7) == "newmtl ")
			{
				inputString >> identifier >> tempMInfo.name;
				cont = true;
				inputString = std::istringstream();
				while (cont)
				{
					std::getline(file, line);
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
						if (line.substr(0, 6) == "illum ")
						{
							inputString >> identifier >> tempMInfo.illum;
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
						cont = false;
					}

				}
			}
		}

	}

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

	ZeroMemory(&this->vertexBuffer, sizeof(ID3D11Buffer));

	this->gDevice->CreateBuffer(&bufferDesc, &data, &this->vertexBuffer);


	UINT32 vertexSize = sizeof(vertexInfo);
	UINT32 offset = 0;
	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &vertexSize, &offset);
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

	gDeviceContext->OMSetRenderTargets(NROFBUFFERS, this->renderTargetViews, this->DSV);
	

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


	gDeviceContext->Draw(36, 0);
	this->swapChain->Present(0, 0);
}

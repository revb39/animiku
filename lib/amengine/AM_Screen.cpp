/************************************************************************
*	AM_Screen.cpp -- Main interface to AMENGINE							*
*					 Copyright (c) 2014 Justin Byers					*
* ===================================================================== *
* This file is part of AniMiku.											*
*																		*
* AniMiku is free software : you can redistribute it and / or modify	*
* it under the terms of the GNU General Public License as published by	*
* the Free Software Foundation, either version 3 of the License, or		*
* (at your option) any later version.									*
*																		*
* AniMiku is distributed in the hope that it will be useful,			*
* but WITHOUT ANY WARRANTY; without even the implied warranty of		*
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the			*
* GNU General Public License for more details.							*
*																		*
* You should have received a copy of the GNU General Public License		*
* along with AniMiku. If not, see <http://www.gnu.org/licenses/>.		*
*************************************************************************/

#include "AM_Screen.h"
#include "am_resource.h"
#include "AM_VMD.h"
#include "AM_AMD.h"

/*The screen singleton*/
Screen ScreenBase::theScreen;

/*Callback window mesage handler*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
	case WM_CLOSE:
		break;
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}

Screen::Screen()
{
	_badInit = false;

	//register all the built in type parsers
	RegisterModelType("pmd", "PMD Model", AM_Pmd);
	RegisterModelType("apmd", "AniMiku Extended PMD Model", AM_Pmd);
	RegisterModelType("pmx", "PMX Model", AM_Pmx);

	RegisterMotionType("vmd", "Vocaloid Motion Data", AM_Vmd);
	RegisterMotionType("amd", "AniMiku Motion Data", AM_Amd);

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChain = NULL;
	d3ddev = NULL;
	backBuffer = NULL;
	renderTargetView = NULL;

	ZeroMemory(&layout, sizeof(layout));
	numElements = 0;

	rasterState = NULL;

	DepthStencilView = NULL;
	DepthStencilBuffer = NULL;

	effect = NULL;
	effectTech = NULL;
	vertexLayout = NULL;

	shadowMap = NULL;
	shadowMapDepthView = NULL;
	shadowMapSRView = NULL;
	shadowMapTech = NULL;

	posMap = NULL;
	normalMap = NULL;
	depthMap = NULL;
	defferredDepthStencil = NULL;
	ssaoMap = NULL;
	blur1Map = NULL;
	posMapSRView = NULL;
	normalMapSRView = NULL;
	depthMapSRView = NULL;
	ssaoMapSRView = NULL;
	blur1MapSRView = NULL;
	posMapRenderTargetView = NULL;
	normalMapRenderTargetView = NULL;
	depthMapRenderTargetView = NULL;
	ssaoMapRenderTargetView = NULL;
	blur1MapRenderTargetView = NULL;
	defferredDepthStencilView = NULL;
	randomTexture = NULL;
	ssaoMapTech = NULL;
	ssaoOcclusionTech = NULL;
	blur1Tech = NULL;
}
Screen::~Screen()
{
	if (d3ddev)
		d3ddev->ClearState();
	if (renderTargetView)
	{
		renderTargetView->Release();
		ZeroMemory(&renderTargetView, sizeof(renderTargetView));
	}
	if (swapChain)
	{
		swapChain->Release();
		ZeroMemory(&swapChain, sizeof(swapChain));
	}
	if (DepthStencilView)
	{
		DepthStencilView->Release();
		ZeroMemory(&DepthStencilView, sizeof(DepthStencilView));
	}
	if (DepthStencilBuffer)
	{
		DepthStencilBuffer->Release();
		ZeroMemory(&DepthStencilBuffer, sizeof(DepthStencilBuffer));
	}
	if (d3ddev)
	{
		d3ddev->Release();
		ZeroMemory(&d3ddev, sizeof(d3ddev));
	}
	if (vertexLayout)
	{
		vertexLayout->Release();
		ZeroMemory(&vertexLayout, sizeof(vertexLayout));
	}
	if (effect)
	{
		effect->Release();
		ZeroMemory(&effect, sizeof(effect));
	}
	if (shadowMap)
	{
		shadowMap->Release();
		ZeroMemory(&shadowMap, sizeof(shadowMap));
	}
	if (shadowMapDepthView)
	{
		shadowMapDepthView->Release();
		ZeroMemory(&shadowMapDepthView, sizeof(shadowMapDepthView));
	}
	if (shadowMapSRView)
	{
		shadowMapSRView->Release();
		ZeroMemory(&shadowMapSRView, sizeof(shadowMapSRView));
	}
	if (posMap)
	{
		posMap->Release();
		ZeroMemory(&posMap, sizeof(posMap));
	}
	if (normalMap)
	{
		normalMap->Release();
		ZeroMemory(&normalMap, sizeof(normalMap));
	}
	if (depthMap)
	{
		depthMap->Release();
		ZeroMemory(&depthMap, sizeof(depthMap));
	}
	if (ssaoMap)
	{
		ssaoMap->Release();
		ZeroMemory(&ssaoMap, sizeof(ssaoMap));
	}
	if (blur1Map)
	{
		blur1Map->Release();
		ZeroMemory(&blur1Map, sizeof(blur1Map));
	}
	if (defferredDepthStencil)
	{
		defferredDepthStencil->Release();
		ZeroMemory(&defferredDepthStencil, sizeof(defferredDepthStencil));
	}
	if (posMapSRView)
	{
		posMapSRView->Release();
		ZeroMemory(&posMapSRView, sizeof(posMapSRView));
	}
	if (normalMapSRView)
	{
		normalMapSRView->Release();
		ZeroMemory(&normalMapSRView, sizeof(normalMapSRView));
	}
	if (depthMapSRView)
	{
		depthMapSRView->Release();
		ZeroMemory(&depthMapSRView, sizeof(depthMapSRView));
	}
	if (ssaoMapSRView)
	{
		ssaoMapSRView->Release();
		ZeroMemory(&ssaoMapSRView, sizeof(ssaoMapSRView));
	}
	if (blur1MapSRView)
	{
		blur1MapSRView->Release();
		ZeroMemory(&blur1MapSRView, sizeof(blur1MapSRView));
	}
	if (posMapRenderTargetView)
	{
		posMapRenderTargetView->Release();
		ZeroMemory(&posMapRenderTargetView, sizeof(posMapRenderTargetView));
	}
	if (normalMapRenderTargetView)
	{
		normalMapRenderTargetView->Release();
		ZeroMemory(&normalMapRenderTargetView, sizeof(normalMapRenderTargetView));
	}
	if (depthMapRenderTargetView)
	{
		depthMapRenderTargetView->Release();
		ZeroMemory(&depthMapRenderTargetView, sizeof(depthMapRenderTargetView));
	}
	if (defferredDepthStencilView)
	{
		defferredDepthStencilView->Release();
		ZeroMemory(&defferredDepthStencilView, sizeof(defferredDepthStencilView));
	}
	if (ssaoMapRenderTargetView)
	{
		ssaoMapRenderTargetView->Release();
		ZeroMemory(&ssaoMapRenderTargetView, sizeof(ssaoMapRenderTargetView));
	}
	if (blur1MapRenderTargetView)
	{
		blur1MapRenderTargetView->Release();
		ZeroMemory(&blur1MapRenderTargetView, sizeof(blur1MapRenderTargetView));
	}
	if (randomTexture)
	{
		randomTexture->Release();
		ZeroMemory(&randomTexture, sizeof(randomTexture));
	}

	ModelList::iterator it, it2;

	for (it = _mdlList.begin(); it != _mdlList.end(); it++)
	{
		if ((*it).second)
		{
			//find copies and null them
			for (it2 = std::next(it); it2 != _mdlList.end(); it2++)
			{
				if ((*it2).second == (*it).second)
					it2->second = NULL;
			}
			if ((*it).second->model)
			{
				if ((*it).second->model->getResult(true))
					delete (*it).second->model->getResult(true);
				delete (*it).second->model;
			}
			if ((*it).second->file)
				delete[](*it).second->file;
			delete (*it).second;
			it->second = NULL;
		}
	}

	MotionList::iterator mit;

	for (mit = _motList.begin(); mit != _motList.end(); mit++)
	{
		if ((*mit).second)
		{
			if ((*mit).second->getResult(true))
				delete (*mit).second->getResult(true);
			delete (*mit).second;
		}
	}

	if (_pDebug)
		delete _pDebug;
	if (cam)
		delete cam;
	if (light)
		delete light;
}
bool Screen::createSCandDevice()
{
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = rndrWindow;
	swapChainDesc.SampleDesc.Count = settings.getNumSamples();
	swapChainDesc.SampleDesc.Quality = settings.getNumSamples()-1;
	swapChainDesc.Windowed = TRUE;
	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS | D3D10_CREATE_DEVICE_SINGLETHREADED||D3D10_CREATE_DEVICE_DEBUG,
		D3D10_SDK_VERSION, &swapChainDesc,
		&swapChain, &d3ddev);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Error initializing Direct3D\n\n*This is usually caused by the configuration being unsupported by your graphics card, or your graphics card not supporting DirectX 10.\n\n*Try lowering and/or disabling some settings in the software config.\n\n*This may also have been caused by outdated graphics card drivers. Please make sure your graphics drivers are up to date.\n\n*If you are running a system with Nvidia Optimus, please make sure the software is running on the dedicated GPU.\n\n*Unfortunately, if your graphics card does not support DirectX 10, the only fix for this would be to use one that does.", "Fatal Error", MB_OK);
		return false;
	}
	return true;
}
void Screen::createRT()
{
	swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBuffer);
	d3ddev->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	backBuffer->Release();
	d3ddev->OMSetRenderTargets(1, &renderTargetView, NULL);
}
void Screen::createVP()
{
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	d3ddev->RSSetViewports(1, &vp);

	D3D10_RECT rects[1];
	rects[0].left = 0;
	rects[0].right = width;
	rects[0].top = 0;
	rects[0].bottom = height;
	d3ddev->RSSetScissorRects(1, rects);

	rasterDesc.FillMode = D3D10_FILL_SOLID;
	rasterDesc.CullMode = D3D10_CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = false;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.ScissorEnable = true;
	rasterDesc.MultisampleEnable = settings.getEnabledMSAA();
	rasterDesc.AntialiasedLineEnable = false;
	d3ddev->CreateRasterizerState(&rasterDesc, &rasterState);
	d3ddev->RSSetState(rasterState);
}
void Screen::createDS()
{
	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = settings.getNumSamples();
	depthStencilDesc.SampleDesc.Quality = settings.getNumSamples()-1;
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	d3ddev->CreateTexture2D(&depthStencilDesc, NULL, &DepthStencilBuffer);
	d3ddev->CreateDepthStencilView(DepthStencilBuffer, NULL, &DepthStencilView);
	d3ddev->OMSetRenderTargets(1, &renderTargetView, DepthStencilView);
}
void Screen::createEffect()
{
	ID3D10Blob *pErrorMsgs = NULL;
	//get the compiled shader from the program resources
	AMResource shaderRes(":/shaders/shaders/AMShader.fxo");
	if (!shaderRes.Open())
	{
		MessageBox(NULL,"3. Error loading shader", "Error", MB_OK);
		exit(0);
	}
	shaderRes.Read();

	HRESULT hr = D3DX10CreateEffectFromMemory(shaderRes.GetData(), shaderRes.GetSize(), "AM SHADER", NULL, NULL, "fx_4_0", NULL, 0,
		d3ddev, NULL, NULL, &effect, &pErrorMsgs, NULL);

	shaderRes.Close();
	shaderRes.Free();
	
	
	if(FAILED(hr) && (pErrorMsgs != NULL))		//Failed to create Effect
	{
		char* msg = (char*)pErrorMsgs->GetBufferPointer();
		MessageBox(NULL, msg, "", MB_OK);
		exit(0);
	}
}
void Screen::parseEffect()
{
	view = effect->GetVariableByName("viewMatrix")->AsMatrix();
	projection = effect->GetVariableByName("projMatrix")->AsMatrix();
	lightViewProj = effect->GetVariableByName("lightViewProjMatrix")->AsMatrix();
	cameraPos = effect->GetVariableByName("cameraPos")->AsVector();
	lightDirec = effect->GetVariableByName("lightDir")->AsVector();
	renderingSkel = effect->GetVariableByName("renderingSkel")->AsScalar();
	extraBright = effect->GetVariableByName("extraBright")->AsScalar();

	texelSize = effect->GetVariableByName("texelSize")->AsVector();

	rigidBodyTrans = effect->GetVariableByName("rigidBodyTrans")->AsMatrix();
	renderingRBody = effect->GetVariableByName("renderingRBody")->AsScalar();

	effectTech = effect->GetTechniqueByName("Skinning");
	effectTech->GetPassByIndex(0)->GetDesc(&passDesc);

	shadowMapTech = effect->GetTechniqueByName("ShadowMapT");
	shadowMapTech->GetPassByIndex(0)->GetDesc(&passDescShadow);

	ssaoMapTech = effect->GetTechniqueByName("SSAO_T");
	ssaoMapTech->GetPassByIndex(0)->GetDesc(&passDescSSAO);

	ssaoOcclusionTech = effect->GetTechniqueByName("SSAO_OCC_T");
	ssaoOcclusionTech->GetPassByIndex(0)->GetDesc(&passDescSSAOOcclusion);

	blur1Tech = effect->GetTechniqueByName("BLUR1_T");
	blur1Tech->GetPassByIndex(0)->GetDesc(&passDescBlur1);
}
void Screen::createVL()
{
	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "NORMAL";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	layout[2].SemanticName = "TEXCOORD";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;

	layout[3].SemanticName = "BONELINKS";
	layout[3].SemanticIndex = 0;
	layout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[3].InputSlot = 0;
	layout[3].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[3].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[3].InstanceDataStepRate = 0;

	layout[4].SemanticName = "BONEWEIGHTS";
	layout[4].SemanticIndex = 0;
	layout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[4].InputSlot = 0;
	layout[4].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[4].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[4].InstanceDataStepRate = 0;

	numElements = sizeof(layout)/sizeof(layout[0]);
	
	d3ddev->CreateInputLayout(layout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &vertexLayout);
	d3ddev->IASetInputLayout(vertexLayout);

}
void Screen::createSM()
{
	//create shadow map texture desc
	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Create the depth stencil view desc
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	//create shader resource view desc
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//create texture and depth/resource views
	d3ddev->CreateTexture2D( &texDesc, NULL, &shadowMap);
	d3ddev->CreateDepthStencilView( shadowMap, &descDSV, &shadowMapDepthView);
	d3ddev->CreateShaderResourceView( shadowMap, &srvDesc, &shadowMapSRView);
}
void Screen::createSSAO()
{
	//create ssao map texture desc
	D3D10_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

	//create shader resource view desc
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// Setup the description of the render target view
	D3D10_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view desc
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	/*Step 1: create buffers for the position map*/
	//
	//create texture 
	d3ddev->CreateTexture2D( &texDesc, NULL, &posMap);
	//create render target view
	d3ddev->CreateRenderTargetView(posMap, &renderTargetViewDesc, &posMapRenderTargetView);
	//create shader resource view
	d3ddev->CreateShaderResourceView( posMap, &srvDesc, &posMapSRView);
	
	/*Step 2: create buffers for the normal map*/
	//
	//create texture 
	d3ddev->CreateTexture2D( &texDesc, NULL, &normalMap);
	//create render target view
	d3ddev->CreateRenderTargetView(normalMap, &renderTargetViewDesc, &normalMapRenderTargetView);
	//create shader resource view
	d3ddev->CreateShaderResourceView( normalMap, &srvDesc, &normalMapSRView);
	
	/*Step 3: create buffers for the depth map*/
	//
	//create texture 
	d3ddev->CreateTexture2D( &texDesc, NULL, &depthMap);
	//create render target view
	d3ddev->CreateRenderTargetView(depthMap, &renderTargetViewDesc, &depthMapRenderTargetView);
	//create shader resource view
	d3ddev->CreateShaderResourceView( depthMap, &srvDesc, &depthMapSRView);

	/*Step 4: create the ssao occlusion map buffers*/
	//
	//create texture
	d3ddev->CreateTexture2D(&texDesc, NULL, &ssaoMap);
	//create render target view
	d3ddev->CreateRenderTargetView(ssaoMap, &renderTargetViewDesc, &ssaoMapRenderTargetView);
	//create shader resource view
	d3ddev->CreateShaderResourceView(ssaoMap, &srvDesc, &ssaoMapSRView);

	/*Step 5: create the ssao occlusion map buffers*/
	//
	//create texture
	d3ddev->CreateTexture2D(&texDesc, NULL, &blur1Map);
	//create render target view
	d3ddev->CreateRenderTargetView(blur1Map, &renderTargetViewDesc, &blur1MapRenderTargetView);
	//create shader resource view
	d3ddev->CreateShaderResourceView(blur1Map, &srvDesc, &blur1MapSRView);

	/*Step 6: create the depth stencil view for depth testing*/
	//
	//update the texture description
	texDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	//create the depth stencil texture
	d3ddev->CreateTexture2D( &texDesc, NULL, &defferredDepthStencil);
	//create the depth stencil view
	d3ddev->CreateDepthStencilView(defferredDepthStencil, &descDSV, &defferredDepthStencilView);

	loadNoiseTexture();
}
void Screen::loadNoiseTexture()
{
	//get the noise texture from the program resources
	AMResource noiseTex(":/shaders/noise.jpg");
	size_t res_size;

	if (!noiseTex.Open())
	{
		MessageBox(NULL, "1: Error Loading Shader", "Fatal Error", MB_OK);
		exit(0);
	}
	noiseTex.Read();

	char* memImg = noiseTex.GetData();
	res_size = noiseTex.GetSize();

	//texture load info
	D3DX10_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory(&loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO));
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	loadInfo.Format = DXGI_FORMAT_BC1_UNORM;

	D3DX10CreateShaderResourceViewFromMemory(d3ddev, memImg, res_size, &loadInfo, NULL, &randomTexture, NULL);

	noiseTex.Close();
	noiseTex.Free();
}
void Screen::initD3D()
{
	//do all D3D initialization
	if (!createSCandDevice())
	{
		_badInit = true;
		return;
	}
	createRT();
	createVP();
	createDS();
	createSM();
	createSSAO();
	createEffect();
	parseEffect();
	createVL();
	//create the light
	light = new AMLight(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	//set up the physics engine
	physics.setup(30, settings.getGravityMul());
	_pDebug = new AMPhysicsDebug(d3ddev, effectTech);
	//only wireframe supported for physics debug
	_pDebug->setDebugMode(AMPhysicsDebug::DBG_DrawWireframe);
	physics.getWorld()->setDebugDrawer(_pDebug);
}
void Screen::init(int w, int h, HWND hwnd)
{
	//store the render window handle
	rndrWindow = hwnd;

	//get the directory of the application & set as CWD
	char dir[MAX_PATH];
	strcpy_s(dir, sizeof(dir), GetCommandLine());
	while (dir[strlen(dir)-1] != '\\')
	{
		dir[strlen(dir)-1] = '\0';
	}
	SetCurrentDirectory(dir);

	//show console if set in settings
	if (settings.getDevCon())
		allocConsole();

	//dimension parameters can override ones in settings
	if (w == 0)
		width = settings.getWidth();
	else
		width = w;
	if (h == 0)
		height = settings.getHeight();
	else
		height = h;
	
	//initialize D3D
	initD3D();

	//create the camera and set to default orientation
	cam = new Camera(width, height);
	cam->lookAt(0, 10, 0);
}

void Screen::registerManagerCallback(AM_Manager* mgr)
{
	_mgr = mgr;
}

void Screen::processWindowMsg()
{
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
void Screen::render()
{
	//process any window messages
	processWindowMsg();

	//don't render the model if D3D initialization failed
	if (_badInit)
		return;
	
	//update the physics simulation to the current time
	physics.update();

	//set the view and projection matrices
	view->SetMatrix((float*)&(cam->matView));
	projection->SetMatrix((float*)&(cam->matProjection));
	//set the camera position
	D3DXVECTOR3 c = D3DXVECTOR3(cam->getPos());
	cameraPos->SetFloatVector((float*)&c);
	//set the light information
	lightDirec->SetFloatVector(light->getDirection());
	lightViewProj->SetMatrix((float*)(light->getViewMat()*light->getProjMat()));
	effect->GetVariableByName("lightViewMatrix")->AsMatrix()->SetMatrix(light->getViewMat());
	effect->GetVariableByName("lightPos")->AsVector()->SetFloatVector(light->getPos());

	//set shadow mapping on/off
	effect->GetVariableByName("shadows")->AsScalar()->SetBool(settings.getShadows());
	//set minimal render on/off
	effect->GetVariableByName("minimal")->AsScalar()->SetBool(settings.getMinimal());

	//set the texel (screen) size
	float ts[2];
	ts[0] = (float)width;
	ts[1] = (float)height;
	texelSize->SetFloatVector((float*)ts);
	

	ModelList::iterator it;
	Model* cur;

	//generate shadow map render pass
	if (settings.getShadows())
	{
		//shadow map generation
		d3ddev->OMSetRenderTargets(0, 0, shadowMapDepthView);
		d3ddev->ClearDepthStencilView(shadowMapDepthView, D3D10_CLEAR_DEPTH, 1.0f, 0);
		for (it=_mdlList.begin();it!=_mdlList.end();it++)
		{	
			cur = (*it).second;
			if (cur == NULL)
				continue;

			if (cur->visible || (cur->model->getResult(true) != NULL && !cur->model->getResult(false)->getFader()->GetFadingDone()))
			{
				cur->model->getResult(true)->render(&settings, shadowMapTech);
			}
		}
		effect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource( shadowMapSRView );
	}
	//generate SSAO pos, normal & depth maps pass
	effect->GetVariableByName("SSAO")->AsScalar()->SetBool(settings.getSSAO());
	if (settings.getSSAO())
	{
		//set up multiple render targets
		ID3D10RenderTargetView *arr[3];
		arr[0] = posMapRenderTargetView;
		arr[1] = normalMapRenderTargetView;
		arr[2] = depthMapRenderTargetView;
		d3ddev->OMSetRenderTargets(3, arr, defferredDepthStencilView);
		//set SSAO constants
		effect->GetVariableByName("samples")->AsScalar()->SetFloat((float)settings.getSSAOSamples());
		effect->GetVariableByName("ssaoRad")->AsScalar()->SetFloat(settings.getSSAORad());
		effect->GetVariableByName("ssaoIntensity")->AsScalar()->SetFloat(settings.getSSAOInten());
		effect->GetVariableByName("ssaoScale")->AsScalar()->SetFloat(settings.getSSAOScale());
		effect->GetVariableByName("ssaoBias")->AsScalar()->SetFloat(settings.getSSAOBias());

		d3ddev->ClearRenderTargetView(posMapRenderTargetView, D3DXVECTOR4(0, 0, 0, 1));
		d3ddev->ClearRenderTargetView(normalMapRenderTargetView, D3DXVECTOR4(0, 0, 0, 1));
		d3ddev->ClearRenderTargetView(depthMapRenderTargetView, D3DXVECTOR4(0, 0, 0, 1));

		d3ddev->ClearDepthStencilView(defferredDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
		for (it=_mdlList.begin();it!=_mdlList.end();it++)
		{	
			cur = (*it).second;
			if (cur == NULL)
				continue;

			if (cur->visible || (cur->model->getResult(true) != NULL && !cur->model->getResult(false)->getFader()->GetFadingDone()))
			{
				cur->model->getResult(true)->render(&settings, ssaoMapTech);
			}
		}
		
		//set just rendered maps
		effect->GetVariableByName("posMap")->AsShaderResource()->SetResource( posMapSRView );
		effect->GetVariableByName("normalMap")->AsShaderResource()->SetResource( normalMapSRView );
		effect->GetVariableByName("depthMap")->AsShaderResource()->SetResource( depthMapSRView );
		effect->GetVariableByName("randomTexture")->AsShaderResource()->SetResource( randomTexture );

		//render the SSAO occlusion map
		d3ddev->OMSetRenderTargets(1, &ssaoMapRenderTargetView, defferredDepthStencilView);
		d3ddev->ClearRenderTargetView(ssaoMapRenderTargetView, D3DXVECTOR4(0, 0, 0, 1));
		d3ddev->ClearDepthStencilView(defferredDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
		for (it=_mdlList.begin();it!=_mdlList.end();it++)
		{	
			cur = (*it).second;
			if (cur == NULL)
				continue;

			if (cur->visible || (cur->model->getResult(true) != NULL && !cur->model->getResult(false)->getFader()->GetFadingDone()))
			{
				cur->model->getResult(true)->render(&settings, ssaoOcclusionTech);
			}
		}
		//set the newly rendered occlusion map
		effect->GetVariableByName("occlusionMap")->AsShaderResource()->SetResource(ssaoMapSRView);

		//stage 1 blur (stage 2 done in main render pass)
		d3ddev->OMSetRenderTargets(1, &blur1MapRenderTargetView, defferredDepthStencilView);
		d3ddev->ClearRenderTargetView(blur1MapRenderTargetView, D3DXVECTOR4(0, 0, 0, 1));
		d3ddev->ClearDepthStencilView(defferredDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
		for (it=_mdlList.begin();it!=_mdlList.end();it++)
		{	
			cur = (*it).second;
			if (cur == NULL)
				continue;

			if (cur->visible || (cur->model->getResult(true) != NULL && !cur->model->getResult(false)->getFader()->GetFadingDone()))
			{
				cur->model->getResult(true)->render(&settings, blur1Tech);
			}
		}
		//set the blurred SSAO map in the shader
		effect->GetVariableByName("occlusionMap")->AsShaderResource()->SetResource(blur1MapSRView);
	}

	//render final scene
	d3ddev->OMSetRenderTargets(1, &renderTargetView, DepthStencilView);
	d3ddev->ClearRenderTargetView(renderTargetView, D3DXVECTOR4(0, 0, 0, 1));
	d3ddev->ClearDepthStencilView(DepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
	
	for (it=_mdlList.begin();it!=_mdlList.end();it++)
	{	
		cur = (*it).second;
		if (cur == NULL)
			continue;

		//render physics debug if flag is set
		if (cur->physVisible)
		{
			renderingRBody->SetBool(true);
			physics.getWorld()->debugDrawWorld();
		}

		if (cur->visible || (cur->model->getResult(true) != NULL && !cur->model->getResult(false)->getFader()->GetFadingDone())) //normal render
		{
			renderingSkel->SetBool(false);
			renderingRBody->SetBool(false);
			
			cur->model->getResult(true)->render(&settings, effectTech);

			//trigger event in manager if motion ended
			if (cur->model->getResult(true)->assignedMotion != NULL && cur->model->getResult(true)->assignedMotion->getStopped())
				_mgr->songEnd();
		}
		else if (!cur->visible && (cur->model->getResult(true) != NULL && cur->model->getResult(false)->getFader()->GetFadingDone()) && cur->fadingOut)
		{
			//model is no longer visible, the fading out is done, but the fadingout variable hasn't been reset
			//trigger event in manager for fade out complete
			_mgr->fadedOut();
			cur->fadingOut = false;
			cur->model->getResult(true)->setPhysicsEnabled(false);
		}
	}

	//clear map variables in shader
	effect->GetVariableByName("shadowMap")->AsShaderResource()->SetResource(0);
	effect->GetVariableByName("posMap")->AsShaderResource()->SetResource(0);
	effect->GetVariableByName("normalMap")->AsShaderResource()->SetResource(0);
	effect->GetVariableByName("depthMap")->AsShaderResource()->SetResource(0);
	effectTech->GetPassByIndex(0)->Apply(0);
	//flip the swap chain to display newly rendered frame
	swapChain->Present(1, 0);
}
struct ModelLoadData
{
	char* file;
	ID3D10Device *dev;
	ID3D10Effect *eff;
	ID3D10EffectTechnique *effT;
	BulletPhysics *phys;
};
void cacheLoadModel(AM_Model* pMdl, void* pParams)
{
	ModelLoadData *d = (ModelLoadData*)pParams;
	printf("Loading model: ");
	printf(d->file);
	printf("\n");
	if (!pMdl->load(d->file, d->dev, d->phys))
		return;
	pMdl->updateDevice(d->dev, d->eff, d->effT);
	delete d;
}
void Screen::preloadModel(char *id, char *file)
{
	if (id == NULL || strlen(id) < 1)
		return;
	if (file == NULL || strlen(file) < 1)
		return;

	//check if model was already loaded
	ModelList::iterator mit = _mdlList.begin();
	for (mit; mit != _mdlList.end(); mit++)
	{
		if (!strcmp((*mit).second->file, file))
		{
			_mdlList[id] = (*mit).second; //create reference to already loaded model
			return; //skip the load operation
		}
	}

	Model *tmpMdl = new Model();

	strcpy_s(tmpMdl->id, sizeof(tmpMdl->id), id);

	tmpMdl->file = new char[strlen(file)+1];
	strcpy_s(tmpMdl->file, strlen(file)+1, file);

	tmpMdl->visible = false;
	tmpMdl->physVisible = false;

	//create the struct to hold data to be sent to the load function
	ModelLoadData *mdlLoadData = new ModelLoadData();
	mdlLoadData->file = file;
	mdlLoadData->dev = d3ddev;
	mdlLoadData->eff = effect;
	mdlLoadData->effT = effectTech;
	mdlLoadData->phys = &physics;

	//create the cache object, passing in the AM_Model we need for this file type
	tmpMdl->model = new AMCache<AM_Model>((void*)mdlLoadData, AM_Model::CreateFromType(AMUtils::getExtension(file)), cacheLoadModel);

	_mdlList[id] = tmpMdl;

	//run the cache operation to load the model
	_mdlList[id]->model->run();

	_mdlList[id]->model->getResult(true); //delay to wait until loaded
}

void Screen::resetModelPhysics(char* id)
{
	_mdlList[id]->model->getResult(true)->resetPhysics();
}

void Screen::clearModel(char* id)
{
	ModelList::iterator i = _mdlList.find(id);
	if (i == _mdlList.end())
		return;
	
	if (_mdlList[id])
	{
		if (_mdlList[id]->model)
		{
			if (_mdlList[id]->model->getResult(true))
				delete _mdlList[id]->model->getResult(true);
			delete _mdlList[id]->model;
			delete _mdlList[id]->file;
		}
		delete _mdlList[id];
	}
	_mdlList.erase(i);
}
void Screen::clearModels()
{
	ModelList::iterator it;

	for (it=_mdlList.begin();it!=_mdlList.end();it++)
	{
		clearModel((char*)(*it).first.c_str());
	}
}
bool Screen::modelExists(char* id)
{
	return _mdlList.find(id) != _mdlList.end();
}
void Screen::setModelVisible(char* id, bool v)
{
	if (_mdlList.find(id) == _mdlList.end())
		return;

	Model *cur;
	cur = _mdlList[id];
	if (cur == NULL)
		return;

	if (v == cur->visible)
		return;

	cur->visible = v;
	cur->model->getResult(true)->setAlphaFade(settings.getEcchi());
	cur->model->getResult(true)->getFader()->BeginFade(settings.getFadeLength(), !v);
	
	if (v)
		cur->model->getResult(true)->setPhysicsEnabled(v);
	else
		cur->fadingOut = true;
}
bool Screen::getModelVisible(char* id)
{
	return _mdlList[id] != NULL && _mdlList[id]->visible;
}
void Screen::setPhysVisible(char* id, bool v)
{
	if (_mdlList.find(id) == _mdlList.end())
		return;

	if (_mdlList[id] == NULL)
		return;

	_mdlList[id]->physVisible = v;
}

void Screen::rotateModel(char* id, float x, float y, float z)
{
	if (_mdlList.find(id) == _mdlList.end())
		return;

	AM_Model *mdl = _mdlList[id]->model->getResult(false);
	if (mdl == NULL)
		return;


	mdl->rotate(x*(PI/180)+PI, y*(PI/180)+PI, z*(PI/180)+PI);
}
void Screen::moveModel(char* id, float x, float y, float z)
{
	if (_mdlList.find(id) == _mdlList.end())
		return;

	AM_Model *mdl = _mdlList[id]->model->getResult(false);
	if (mdl == NULL)
		return;

	mdl->translate(x, y, z);
}

void cacheLoadMotion(AM_Motion* pMotion, void* pParam)
{
	printf((char*)pParam);
	pMotion->load((char*)pParam);
	delete[] pParam;
}
void Screen::preloadMotion(char* id, char* file)
{
	char* f = new char[strlen(file)+1];
	strcpy_s(f, strlen(file)+1, file);
	_motList[id] = new AMCache<AM_Motion>(f, AM_Motion::CreateFromType(AMUtils::getExtension(f)), cacheLoadMotion);
	_motList[id]->run();
}

void Screen::playMotion(char* mdlID)
{
	_mdlList[mdlID]->model->getResult(true)->startMotion();
}
void Screen::stopMotion(char* mdlID)
{
	_mdlList[mdlID]->model->getResult(true)->stopMotion();
}
void Screen::assignMotionToModel(char* mdlID, char* motID, bool force)
{
	bool reset = !_mdlList[mdlID]->visible || (_mdlList[mdlID]->model->getResult(true)->assignedMotion == _motList[motID]->getResult(true));
	_mdlList[mdlID]->model->getResult(true)->assignMotion(_motList[motID]->getResult(true));
	if (reset || force) //the animation is the same, or the model is invisible
	{
		_mdlList[mdlID]->model->getResult(true)->moveToDefMotionPos();
	}
}
bool Screen::motionExists(char* motID)
{
	return (_motList.find(motID) != _motList.end());
}
void Screen::clearMotion(char* id)
{
	MotionList::iterator mit = _motList.find(id);
	if (mit != _motList.end() && (*mit).second)
	{
		if ((*mit).second->getResult(true))
				delete (*mit).second->getResult(true);
			delete (*mit).second;
	}
}
void Screen::clearMotions()
{
	MotionList::iterator mit = _motList.begin();
	if (mit != _motList.end() && (*mit).second)
	{
		if ((*mit).second->getResult(true))
				delete (*mit).second->getResult(true);
			delete (*mit).second;
	}
}
bool Screen::motionToAmd(char* file)
{
	AM_Motion* tmp = AM_Motion::CreateFromType(AMUtils::getExtension(file));
	if (tmp->load(file))
	{
		tmp->dumpToAMD();
		delete tmp;
		return true;
	}
	delete tmp;
	return false;
}

AMSettings* Screen::getSettings()
{
	return &settings;
}

void Screen::allocConsole()
{
	int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

	&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

	coninfo.dwSize);

	// redirect unbuffered STDOUT to the console

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

	// point to console as well

	ios::sync_with_stdio();
}
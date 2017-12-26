#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "game.h"

///////////////////////////////////////////////////////////////////////////////////////
//Called every time the application recives a message
//////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);

		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////
//Register class and create window
/////////////////////////////////////////////////////////////////////////
HRESULT game::InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	//Give your app window your own name
	char Name[100] = "Lewis Lock\0";

	//Register lcass
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	//Create window
	g_hInst = hInstance;
	RECT rc = { 0,0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	inputManager = new InputManager();

	inputManager->InitialiseInput(g_hInst, g_hWnd);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////
//Create D3D device and swap chain
////////////////////////////////////////////////////////////////////
HRESULT game::InitialiseD3D()
{
		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(g_hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT createDeviceFlags = 0;

	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE, //Comment out this line fi you need to test D3D 11.0 functionality on hardware that doesn't support it
			D3D_DRIVER_TYPE_WARP, //Comment this out also to use reference device
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = g_hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			g_driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
				createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &g_pSwapChain,
				&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr))
			return hr;

		//Get pointer to back buffer texture
		ID3D11Texture2D *pBackBufferTexture;
		hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

		if (FAILED(hr))
			return hr;

		//Use the back buffer texture pointer to create the render target view
		hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_pBackBufferRTView);

		pBackBufferTexture->Release();

		if (FAILED(hr))
			return hr;

		//Create a Z buffer texture
		D3D11_TEXTURE2D_DESC tex2dDesc;
		ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

		tex2dDesc.Width = width;
		tex2dDesc.Height = height;
		tex2dDesc.ArraySize = 1;
		tex2dDesc.MipLevels = 1;
		tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
		tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

		ID3D11Texture2D *pZBufferTexture;
		hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

		if (FAILED(hr))
			return hr;

		//Create the Z buffer
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));

		dsvDesc.Format = tex2dDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
		pZBufferTexture->Release();

		//Set the render target view
		g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

		//Set the viewport
		D3D11_VIEWPORT viewport;

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		inputManager->SetScreenSize(width, height);

		g_pImmediateContext->RSSetViewports(1, &viewport);

		g_2DText = new Text2D("Assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);

		return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
///////////////////////////////////////////////////////////////////////////
void game::ShutdownD3D()
{
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

///////////////////////////////////////////////////////////////////////////
//Init Graphics
///////////////////////////////////////////////////////////////////////////
HRESULT game::InitialiseGraphics()//03 - 01
{
	HRESULT hr = S_OK;

	g_walls_node = new Scene_node();
	g_sky_node = new Scene_node();
	g_floor_node = new Scene_node();
	g_actors_node = new Scene_node();

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;

	/////////////////SKY BOX///////////////////
	g_pD3DDevice->CreateRasterizerState(&rasterDesc, &g_pRasterSolid);
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	g_pD3DDevice->CreateRasterizerState(&rasterDesc, &g_pRasterSkybox);


	D3D11_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = false;
	depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	g_pD3DDevice->CreateDepthStencilState(&depthDesc, &g_pDepthWriteSolid);
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pD3DDevice->CreateDepthStencilState(&depthDesc, &g_pDepthWriteSkybox);

	g_floor_node->addChildNode(g_actors_node);
	g_actors_node->addChildNode(g_walls_node);

	level1 = new level(false, 12, 12, g_walls_node, g_floor_node, "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);

	player1 = new player(false, 12, 0, 12, g_actors_node, "Assets/PointySphere.obj", "Assets/texture.bmp", "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	AImanager1 = new AImanager(4, g_actors_node, g_floor_node, "Assets/PointySphere.obj", "Assets/texture.bmp", "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	skyBox = new wall(true, 20, 0, 20, g_sky_node, "Assets/cube.obj", "Assets/skybox02.dds", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	skyBox->setScale(42);
	g_sky_node->SetXPos(24);
	g_sky_node->SetZPos(24);
	skyBox->getModel()->SetIsSkybox(true);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////
//Render Frame
/////////////////////////////////////////////////////////////////////////
void game::RenderFrame(void)
{
	inputManager->ReadInputStates();
	if (inputManager->IsKeyPressed(DIK_ESCAPE)) DestroyWindow(g_hWnd);

	//Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(0.4f, 0.4f, 0.4f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.8f, 0.8f, 0.8f, 0.0f);

	XMMATRIX view = player1->getCamera()->GetViewMatrix();

	///////////RENDER FIRST CUBE////////////////////
	XMMATRIX projection;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0, 100.0);

	XMMATRIX identityMatrix = XMMatrixIdentity();

	player1->UpdatePlayer(inputManager, g_actors_node, AImanager1->GetAllBullets(), g_walls_node);

	AImanager1->CheckSpawnEnemies();
	AImanager1->UpdateAllEnemies(player1->GetPlayerBullets(), g_actors_node, player1->getXPos(), player1->getZPos(), g_walls_node, player1);

	//g_sky_node->SetXPos(player1->getXPos());
	//g_sky_node->SetZPos(player1->getZPos());
	g_sky_node->execute(&identityMatrix, &view, &projection, g_directional_light_colour, g_ambient_light_colour, g_directional_light_shines_from);
	g_floor_node->execute(&identityMatrix, &view, &projection, g_directional_light_colour, g_ambient_light_colour, g_directional_light_shines_from);


	string scoreString = string("Score ") + to_string(player1->GetPlayerScore());

	g_2DText->AddText(scoreString, -1.0, +1.0, .1);

	g_2DText->RenderText();

	//Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}
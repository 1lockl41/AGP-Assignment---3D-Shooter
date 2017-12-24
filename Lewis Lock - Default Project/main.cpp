#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "camera.h"
#include "text2D.h"
#include "Model.h"
#include "scene_node.h"
#include "InputManager.h"
#include "baseClass.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "level.h"
#include "wall.h"
#include "AImanager.h"
#include <dinput.h>

int (WINAPIV * __vsnprintf_s)(char *, size_t, const char*, va_list) = _vsnprintf;

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

//////////////////
//Global Variables
//////////////////
HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

//Rename for each tutorial, this string will be displayed as the window title.
char g_TutorialName[100] = "Tutorial 12 Exercise 01\0";

D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device* g_pD3DDevice = NULL;
ID3D11DeviceContext* g_pImmediateContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;

ID3D11RenderTargetView* g_pBackBufferRTView = NULL;

ID3D11DepthStencilView* g_pZBuffer;

ID3D11RasterizerState* g_pRasterSolid;
ID3D11RasterizerState* g_pRasterSkybox;
ID3D11DepthStencilState* g_pDepthWriteSolid;
ID3D11DepthStencilState* g_pDepthWriteSkybox;

Text2D* g_2DText;

XMVECTOR g_directional_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;

InputManager* inputManager;

Scene_node* g_sky_node;  //The root node, contains only the sky box. All collisions ignore this.
Scene_node* g_floor_node; //The second node in the tree, contains the floor. All collisions currently ignore this, as there is no gravity.
Scene_node* g_actors_node; //The third node in the tree, contains all moving actors such as players, enemies and bullets. Player and enemy movement will use this for collision so they dont move through each other.
Scene_node* g_walls_node; //The last node in the last, contains the walls for the level. Bullets will use this for collision so they are destroyed when hitting a wall.

level* level1;
wall* skyBox;

player* player1;
AImanager* AImanager1;

//Define vertex structure
struct POS_COL_TEX_NORM_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

//Const buffer structs. Pack to 16 bytes. Don't let any single element cross a 16 bytes boundary
struct CONSTANT_BUFFER0
{
	XMMATRIX WVPMatrix; //64 bytes
	float RedAmount; //4 bytes 
	float scale; //4 bytes
	XMFLOAT2 packing_bytes; //2x4 bytes = 8 bytes
	XMVECTOR directional_light_vector; //16 bytes
	XMVECTOR directional_light_colour; //16 bytes
	XMVECTOR ambient_light_colour; //16 bytes
}; //128 bytes

//////////////////////
//Forward Declarations
//////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT InitialiseD3D();
void ShutdownD3D();
void RenderFrame(void);

HRESULT InitialiseGraphics(void);
//HRESULT InitialiseInput(void);

/////////////////////////////////////////////////////////////////////
//Entry point to the program. Initialises everything and goes into a
//message processing loop. Idle time is used to render the scene.
/////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Stops compiler warnings about unused variables.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	inputManager = new InputManager();

	inputManager->InitialiseInput(g_hInst, g_hWnd);

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	//Main message loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RenderFrame();
		}
	}

	ShutdownD3D();

	return (int)msg.wParam;
}

/////////////////////////////////////////////////////////////////////////
//Register class and create window
/////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
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

	return S_OK;
}

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
				DestroyWindow(g_hWnd);

			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
//Create D3D device and swap chain
////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
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

	for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, 
			createDeviceFlags, featureLevels, numFeatureLevels, 
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, 
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if(SUCCEEDED(hr))
			break;
	}

	if(FAILED(hr))
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
void ShutdownD3D()
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
HRESULT InitialiseGraphics()//03 - 01
{
	HRESULT hr = S_OK;

	g_walls_node = new Scene_node();
	g_sky_node = new Scene_node();
	g_floor_node = new Scene_node();
	g_actors_node = new Scene_node();

	//Define vertices of a triangle - screen coords -1.0 to +1.0
	POS_COL_TEX_NORM_VERTEX vertices[] =
	{
		//back face
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),   XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),  XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		//front face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		//left face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		//right face
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		//bottom face
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),   XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		//top face
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(1.0f,1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(0.0f,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(1.0f,0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),   XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), XMFLOAT2(0.0f,1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }
	};


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


	g_sky_node->addChildNode(g_floor_node);
	g_floor_node->addChildNode(g_actors_node);
	g_actors_node->addChildNode(g_walls_node);

	level1 = new level(false,20, 20, g_walls_node, g_floor_node, "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);

	player1 = new player(false,20,0,20,g_actors_node, "Assets/PointySphere.obj", "Assets/texture.bmp", "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	AImanager1 = new AImanager(4,g_actors_node, "Assets/PointySphere.obj", "Assets/texture.bmp", "Assets/cube.obj", "Assets/texture.bmp", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	skyBox = new wall(true,20, 0, 20, g_sky_node, "Assets/cube.obj", "Assets/skybox02.dds", g_pD3DDevice, g_pImmediateContext, g_pRasterSolid, g_pRasterSkybox, g_pDepthWriteSolid, g_pDepthWriteSkybox);
	skyBox->setScale(50);
	skyBox->getModel()->SetIsSkybox(true);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////
//Render Frame
/////////////////////////////////////////////////////////////////////////
void RenderFrame(void)
{
	inputManager->ReadInputStates();
	if (inputManager->IsKeyPressed(DIK_ESCAPE)) DestroyWindow(g_hWnd);

	//Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f,0.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(0.4f, 0.4f, 0.4f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.8f, 0.8f, 0.8f, 0.0f);

	XMMATRIX view = player1->getCamera()->GetViewMatrix();

	///////////RENDER FIRST CUBE////////////////////
	XMMATRIX projection;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0, 100.0);

	XMMATRIX identityMatrix = XMMatrixIdentity();

	player1->UpdatePlayer(inputManager, g_actors_node, AImanager1->GetAllBullets(), g_walls_node);

	AImanager1->UpdateAllEnemies(player1->GetPlayerBullets(), g_actors_node, player1->getXPos(), player1->getZPos(), g_walls_node);

	g_sky_node->execute(&identityMatrix, &view, &projection, g_directional_light_colour, g_ambient_light_colour, g_directional_light_shines_from);

	g_2DText->AddText("some text hello", -1.0, +1.0, .1);

	g_2DText->RenderText();

	//Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}
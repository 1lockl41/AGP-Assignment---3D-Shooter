#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "camera.h"
#include "text2D.h"
#include "Model.h"
#include "scene_node.h"
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

ID3D11Buffer* g_pVertexBuffer;
ID3D11VertexShader* g_pVertexShader;
ID3D11PixelShader* g_pPixelShader;
ID3D11InputLayout* g_pInputLayout;
ID3D11Buffer* g_pConstantBuffer0;

ID3D11DepthStencilView* g_pZBuffer;

Camera* camera1;

ID3D11ShaderResourceView* g_pTexture0;
ID3D11SamplerState* g_pSampler0;

Text2D* g_2DText;

XMVECTOR g_directional_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;

Model* model1;
Model* model2;
Model* camera_model;

IDirectInput8* g_direct_input;
IDirectInputDevice8* g_keyboard_device;
unsigned char g_keyboard_keys_state[256];
IDirectInputDevice8* g_mouse_device;
DIMOUSESTATE mouseState;
DIMOUSESTATE mouseStateLast;
int m_screenWidth, m_screenHeight;

Scene_node* g_root_node;
Scene_node* g_node1;
Scene_node* g_node2;
Scene_node* g_camera_node;

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
HRESULT InitialiseInput(void);

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

	if (FAILED(InitialiseInput()))
	{
		DXTRACE_MSG("Failed to initialise input");
		return 0;
	}

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

	m_screenHeight = height;
	m_screenWidth = width;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	g_2DText = new Text2D("Assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
///////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	if (g_pSampler0) g_pSampler0->Release();
	if (g_pTexture0) g_pTexture0->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release(); //03 - 01
	if (g_pInputLayout) g_pInputLayout->Release(); //03 - 01
	if (g_pVertexShader) g_pVertexShader->Release(); //03 - 01
	if (g_pPixelShader) g_pPixelShader->Release(); //03 - 01
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
	if (g_keyboard_device)
	{
		g_keyboard_device->Unacquire();
		g_keyboard_device->Release();
	}
	if (g_mouse_device)
	{
		g_mouse_device->Unacquire();
		g_mouse_device->Release();
	}
}

///////////////////////////////////////////////////////////////////////////
//Init Graphics
///////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics()//03 - 01
{
	HRESULT hr = S_OK;

	g_root_node = new Scene_node();
	g_node1 = new Scene_node();
	g_node2 = new Scene_node();
	g_camera_node = new Scene_node();

	model1 = new Model(g_pD3DDevice, g_pImmediateContext);
	model1->LoadObjModel("Assets/PointySphere.obj");

	model2 = new Model(g_pD3DDevice, g_pImmediateContext);
	model2->LoadObjModel("Assets/PointySphere.obj");

	camera_model = new Model(g_pD3DDevice, g_pImmediateContext);
	camera_model->LoadObjModel("Assets/cube.obj");

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

	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //Used by cpu and gpu
	bufferDesc.ByteWidth = sizeof(vertices); //total size of buffer, 3 vertices
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //Allow cpu access
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer); //Create the buffer

	if (FAILED(hr)) //return error code on failure
	{
		return hr;
	}

	//Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; //can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 128; // MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //Use as a constant buffer

	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))
	{
		return hr;
	}

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	//Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0) //check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) //dont fail if error is just a warning
		{
			return hr;
		};
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//dont fail if error is just a warning
		{
			return hr;
		};
	}

	//Create shader objects
	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

	if (FAILED(hr))
	{
		return hr;
	}

	//Set the shader objects as active
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[]=
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	camera1 = new Camera(0.0f, 0.0f, -0.5f, 0.0f);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pD3DDevice->CreateSamplerState(&sampler_desc, &g_pSampler0);

	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "Assets/texture.bmp", NULL, NULL, &g_pTexture0, NULL);

	model1->SetTexture(g_pTexture0);
	model1->SetSampler(g_pSampler0);
	model2->SetTexture(g_pTexture0);
	model2->SetSampler(g_pSampler0);
	camera_model->SetTexture(g_pTexture0);
	camera_model->SetSampler(g_pSampler0);


	//Scene node management
	g_node1->SetModel(model1);
	g_node2->SetModel(model2);
	g_camera_node->SetModel(camera_model);

	g_root_node->addChildNode(g_node1);
	g_root_node->addChildNode(g_node2);
	g_root_node->addChildNode(g_camera_node);

	g_node1->SetXPos(5);
	g_node1->SetZPos(20);
	g_node2->SetXPos(-5);
	g_node2->SetZPos(20);
	g_camera_node->SetScale(2);

	return S_OK;
}

HRESULT InitialiseInput(void)
{
	HRESULT hr;
	/////////KEYBOARD///////
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	hr = DirectInput8Create(g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;
	////////////////////////

	//////////MOUSE/////////
	hr = DirectInput8Create(g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = g_direct_input->CreateDevice(GUID_SysMouse, &g_mouse_device, NULL);
	if (FAILED(hr)) return hr;

hr = g_mouse_device->SetDataFormat(&c_dfDIMouse);
if (FAILED(hr)) return hr;

hr = g_mouse_device->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
if (FAILED(hr)) return hr;

hr = g_mouse_device->Acquire();
if (FAILED(hr)) return hr;
//////////////////////////

return S_OK;
}

bool IsKeyPressed(unsigned char DI_keycode)
{
	return g_keyboard_keys_state[DI_keycode] & 0x80;
}

void ReadInputStates()
{
	HRESULT hr;
	hr = g_keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_keyboard_device->Acquire();
		}
	}

	hr = g_mouse_device->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			g_mouse_device->Acquire();
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//Render Frame
/////////////////////////////////////////////////////////////////////////
void RenderFrame(void)
{
	ReadInputStates();
	if (IsKeyPressed(DIK_ESCAPE)) DestroyWindow(g_hWnd);

	//Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	XMMATRIX view = camera1->GetViewMatrix();

	///////////RENDER FIRST CUBE////////////////////
	XMMATRIX projection;
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), 640.0 / 480.0, 1.0, 100.0);

	XMMATRIX identityMatrix = XMMatrixIdentity();

	//if (IsKeyPressed(DIK_D)) g_node1->IncX(0.0005f, g_root_node);
	//if (IsKeyPressed(DIK_A)) g_node1->IncX(-0.0005f, g_root_node);
	//if (IsKeyPressed(DIK_W)) g_node1->IncY(0.0005f, g_root_node);
	//if (IsKeyPressed(DIK_S)) g_node1->IncY(-0.0005f, g_root_node);


	if (IsKeyPressed(DIK_DOWN))
	{
		camera1->Pitch(0.0002f);
	}
	if (IsKeyPressed(DIK_UP))
	{
		camera1->Pitch(-0.0002f);
	}
	if (IsKeyPressed(DIK_LEFT))
	{
		camera1->Yaw(-0.0002f);
	}
	if (IsKeyPressed(DIK_RIGHT))
	{
		camera1->Yaw(0.0002f);
	}

	if ((mouseState.lX != mouseStateLast.lX) || (mouseState.lY != mouseStateLast.lY))
	{
		camera1->Yaw(mouseStateLast.lX * 0.001f);
		camera1->Pitch(mouseStateLast.lY * 0.001f);

		mouseStateLast = mouseState;
	}

	if (IsKeyPressed(DIK_W))
	{
		camera1->Forward(0.001f);

		// set camera node to the position of the camera
		g_camera_node->SetXPos(camera1->GetX());
		g_camera_node->SetYPos(camera1->GetY());
		g_camera_node->SetZPos(camera1->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		g_root_node->UpdateCollisionTree(&identity, 1.0);

		if (g_camera_node->check_collision(g_root_node,g_camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			camera1->Forward(-0.001f);
			g_camera_node->SetXPos(camera1->GetX()); 
			g_camera_node->SetYPos(camera1->GetY());
			g_camera_node->SetZPos(camera1->GetZ());

		}
	}
	if (IsKeyPressed(DIK_S))
	{
		camera1->Forward(-0.001f);

		// set camera node to the position of the camera
		g_camera_node->SetXPos(camera1->GetX());
		g_camera_node->SetYPos(camera1->GetY());
		g_camera_node->SetZPos(camera1->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		g_root_node->UpdateCollisionTree(&identity, 1.0);

		if (g_camera_node->check_collision(g_root_node, g_camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			camera1->Forward(0.001f);
			g_camera_node->SetXPos(camera1->GetX());
			g_camera_node->SetYPos(camera1->GetY());
			g_camera_node->SetZPos(camera1->GetZ());
		}
	}
	if (IsKeyPressed(DIK_A))
	{
		camera1->Sideways(-0.001f);

		// set camera node to the position of the camera
		g_camera_node->SetXPos(camera1->GetX());
		g_camera_node->SetYPos(camera1->GetY());
		g_camera_node->SetZPos(camera1->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		g_root_node->UpdateCollisionTree(&identity, 1.0);

		if (g_camera_node->check_collision(g_root_node, g_camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			camera1->Sideways(0.001f);
			g_camera_node->SetXPos(camera1->GetX());
			g_camera_node->SetYPos(camera1->GetY());
			g_camera_node->SetZPos(camera1->GetZ());
		}
	}
	if (IsKeyPressed(DIK_D))
	{
		camera1->Sideways(0.001f);

		// set camera node to the position of the camera
		g_camera_node->SetXPos(camera1->GetX());
		g_camera_node->SetYPos(camera1->GetY());
		g_camera_node->SetZPos(camera1->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		g_root_node->UpdateCollisionTree(&identity, 1.0);

		if (g_camera_node->check_collision(g_root_node, g_camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			camera1->Sideways(-0.001f);
			g_camera_node->SetXPos(camera1->GetX());
			g_camera_node->SetYPos(camera1->GetY());
			g_camera_node->SetZPos(camera1->GetZ());
		}
	}

	g_root_node->execute(&identityMatrix, &view, &projection, g_directional_light_colour, g_ambient_light_colour, g_directional_light_shines_from);

	g_2DText->AddText("some text hello", -1.0, +1.0, .1);

	g_2DText->RenderText();

	//Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}
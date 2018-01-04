#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include "game.h"
#include <dinput.h>

int (WINAPIV * __vsnprintf_s)(char *, size_t, const char*, va_list) = _vsnprintf;

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

/////////////////////////////////////////////////////////////////////
//Entry point to the program. Initialises everything and goes into a
//message processing loop. Idle time is used to render the scene.
/////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Stops compiler warnings about unused variables.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//Set up game class
	game* game1 = NULL;
	game1 = new game();

	if (FAILED(game1->InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(game1->InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(game1->InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	//Main message loop
	MSG msg = { 0 };

	//Enter game loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			game1->RenderFrame(); 
		}
	}

	game1->ShutdownD3D();

	return (int)msg.wParam;
}

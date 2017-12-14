#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <dinput.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

class InputManager
{
private:

	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	unsigned char m_keyboard_keys_state[256];
	IDirectInputDevice8* m_mouse_device;
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_mouseStateLast;
	int m_screenWidth, m_screenHeight;

public:
	InputManager();
	void InitialiseInput(HINSTANCE hInst, HWND hWnd);
	bool IsKeyPressed(unsigned char DI_keycode);
	void ReadInputStates();
	void SetScreenSize(int width, int height);
	DIMOUSESTATE GetMouseState();
	DIMOUSESTATE GetLastMouseState();
	void SetLastMouseState(DIMOUSESTATE state);
};
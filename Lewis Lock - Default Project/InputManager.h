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

//Input manager, handles all inputs from player
class InputManager
{
private:

	//Variables used for keyboard input
	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	unsigned char m_keyboard_keys_state[256];

	//Variables ued for mouse input
	IDirectInputDevice8* m_mouse_device;
	DIMOUSESTATE m_mouseState;
	DIMOUSESTATE m_mouseStateLast;

	int m_screenWidth, m_screenHeight; //Used to keep mouse within game's window

public:
	InputManager();
	void InitialiseInput(HINSTANCE hInst, HWND hWnd); //Initialise input
	bool IsKeyPressed(unsigned char DI_keycode); //Check if a button is pressed on keyboard
	void ReadInputStates(); //Read the current input
	void SetScreenSize(int width, int height); //Set screen size
	DIMOUSESTATE GetMouseState(); //Get current mouse state, allows access to current mouse position
	DIMOUSESTATE GetLastMouseState(); //Get last mouse state, used to determine how the mouse was moved
	void SetLastMouseState(DIMOUSESTATE state); //Set last state of mouse
};
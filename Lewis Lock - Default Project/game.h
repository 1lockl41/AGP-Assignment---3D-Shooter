#pragma once
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
#include "ParticleGenerator.h"
#include "pickupHealth.h"
#include "pickupShotgun.h"
#include "pushableBlock.h"
#include "triggerPlate.h"
#include <dinput.h>

class game
{
private:

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

	Scene_node* g_sky_node;  //A seperate node, contains only the sky box. All collisions ignore this.
	Scene_node* g_floor_node; //The root node in the tree, contains the floor. All collisions currently ignore this, as there is no gravity.
	Scene_node* g_actors_node; //The second node in the tree, contains all moving actors such as players, enemies and bullets. Player and enemy movement will use this for collision so they dont move through each other.
	Scene_node* g_walls_node; //The last node in the last, contains the walls for the level. Bullets will use this for collision so they are destroyed when hitting a wall.

	level* level1;
	wall* skyBox;

	player* player1;
	AImanager* AImanager1;
	pickupHealth* healthKit1;
	pickupShotgun* shotgun1;
	pushableBlock* pushableBlock1;
	triggerPlate* removeableWallsTrigger;

public:

	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HRESULT InitialiseD3D();
	void ShutdownD3D();
	void RenderFrame(void);

	HRESULT InitialiseGraphics(void);

};
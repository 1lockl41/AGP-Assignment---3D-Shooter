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

	//The window title
	char g_TutorialName[100] = "3D Shooter Project\0";

	D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* g_pD3DDevice = NULL;
	ID3D11DeviceContext* g_pImmediateContext = NULL;
	IDXGISwapChain* g_pSwapChain = NULL;

	ID3D11RenderTargetView* g_pBackBufferRTView = NULL;

	ID3D11DepthStencilView* g_pZBuffer = NULL;

	//Variables for drawing skyboxs and solid objects
	ID3D11RasterizerState* g_pRasterSolid = NULL;
	ID3D11RasterizerState* g_pRasterSkybox = NULL;
	ID3D11DepthStencilState* g_pDepthWriteSolid = NULL;
	ID3D11DepthStencilState* g_pDepthWriteSkybox = NULL;

	//2D text which displays player HUD and game over text
	Text2D* g_2DText = NULL;

	//Variables for lighting
	XMVECTOR g_directional_light_shines_from;
	XMVECTOR g_directional_light_colour;
	XMVECTOR g_ambient_light_colour;

	InputManager* inputManager = NULL; //input manager - handles all of the players input

	//A seperate node, contains only the sky box. All collisions ignore this.
	Scene_node* g_sky_node = NULL;
	//The root node in the tree, contains the floor. All collisions currently ignore this, as there is no gravity.
	Scene_node* g_floor_node = NULL;
	//The second node in the tree, contains all moving actors such as players, enemies and bullets. Player and enemy movement will use this for collision so they dont move through each other.
	Scene_node* g_actors_node = NULL;
	//The last node in the last, contains the walls for the level. Actors will use this for collision so they stop when hitting a wall.
	Scene_node* g_walls_node = NULL;

	level* level1 = NULL; //contains level data
	wall* skyBox = NULL; //The skybox

	player* player1 = NULL; //The player
	AImanager* AImanager1 = NULL; //AI manager - spawns and updates enemy AI
	pickupHealth* healthKit1 = NULL; //Healthkit, can be collected to restore health to player
	pickupShotgun* shotgun1 = NULL; //Shotgun upgrade, allows player to fire 3 bullets at once
	pushableBlock* pushableBlock1 = NULL; //A block which can be moved by the player walking into it
	triggerPlate* removeableWallsTrigger = NULL; //A trigger which is activated when the pushable block is pushed over it

	//Varibales for delta timing - maintains consistent game speed across variable frame rates. All timings and speed must be multiplied by the delta time
	double previousTime;
	double deltaTime;

public:

	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	HRESULT InitialiseD3D();
	void ShutdownD3D();
	void RenderFrame(void);

	HRESULT InitialiseGraphics(void);

};
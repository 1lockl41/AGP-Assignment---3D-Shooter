#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"
#include "camera.h"
#include "InputManager.h"
#include "scene_node.h"

class player : public baseClass
{
private:
	Camera* m_playerCamera;



public:

	player(char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{
		m_playerCamera = new Camera(0.0f, 0.0f, -0.5f, 0.0f);
	};

	Camera* getCamera();

	void RotateCamera(InputManager* inputManager);
	void MoveCamera(InputManager* inputManager, Scene_node* camera_node, Scene_node* root_node);
};
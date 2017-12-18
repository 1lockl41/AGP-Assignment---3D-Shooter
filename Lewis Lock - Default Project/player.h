#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"
#include "camera.h"
#include "InputManager.h"
#include "bullet.h"


class player : public baseClass
{
private:
	Camera* m_playerCamera;
	bullet* bullets[10];
	float firingCooldown;
	float firingCooldownReset;

public:

	player(Scene_node* root_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{
		firingCooldownReset = 400;
		firingCooldown = firingCooldownReset;

		m_playerCamera = new Camera(0.0f, 0.0f, -0.5f, 0.0f);

		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(2);
		root_node->addChildNode(m_sceneNode);

		for (int x = 0; x < 10; x++)
		{
			bullets[x] = new bullet(root_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext);
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
		}
	};

	Camera* getCamera();

	void RotateCamera(InputManager* inputManager);
	void MoveCamera(InputManager* inputManager, Scene_node* root_node);

	void UpdateBullets();
	void CheckFiring(InputManager* inputManager);

};
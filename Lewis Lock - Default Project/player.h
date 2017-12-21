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
	//bullet* bullets[10];
	float firingCooldown;
	float firingCooldownReset;
	float m_moveSpeed;
	std::vector<bullet*> bullets;

public:

	player(bool isSkybox, int xPos, int yPos, int zPos, Scene_node* root_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(isSkybox, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		firingCooldownReset = 25;
		firingCooldown = firingCooldownReset;
		m_moveSpeed = 0.25f;

		m_playerCamera = new Camera(getXPos(), getYPos(), getZPos()-0.5f);

		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetBelongsToPlayer(true);


		//m_sceneNode->SetScale(2);
		root_node->addChildNode(m_sceneNode);

		for (int x = 0; x < 10; x++)
		{
			bullets.push_back(new bullet(false, -100,-100,-100,true, root_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox,pDepthWriteSolid, pDepthWrtieSkybox));
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
		}
	};

	Camera* getCamera();

	void RotateCamera(InputManager* inputManager);
	void MoveCamera(InputManager* inputManager, Scene_node* root_node);

	void UpdateBullets(Scene_node* root_node);
	void CheckFiring(InputManager* inputManager);

	std::vector<bullet*> GetPlayerBullets();
};
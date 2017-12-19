#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"
#include "bullet.h"

class enemy : public baseClass
{
private:
	float m_moveSpeed;
	float firingCooldownReset;
	float firingCooldown;
	bullet* bullets[10];

public:

	enemy(Scene_node* root_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{

		firingCooldownReset = 400;
		firingCooldown = firingCooldownReset;
		m_moveSpeed = 0.005f;

		m_sceneNode->SetModel(m_model);
		root_node->addChildNode(m_sceneNode);

		for (int x = 0; x < 10; x++)
		{
			bullets[x] = new bullet(false, root_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext);
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
		}


	};

	void UpdateBullets();

};
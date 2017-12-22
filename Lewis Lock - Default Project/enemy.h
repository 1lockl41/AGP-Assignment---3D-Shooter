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
	float m_firingCooldownReset;
	float m_firingCooldown;
	std::vector<bullet*> bullets;

	int m_currHealth;
	int m_maxHealth;

	int m_damageTaken;
	float m_damageTakenCooldown;
	float m_damageTakenCooldownReset;

	XMVECTOR m_defaultForward;
	XMMATRIX m_rotationMatrix;
	XMVECTOR m_defaultRight;
	XMVECTOR m_defaultUp;
	XMVECTOR m_forward;
	XMVECTOR m_right;
	float m_moveLeftRight;
	float m_moveBackForward;
	XMVECTOR m_lookat, m_up, m_position;

public:

	enemy(bool isSkybox, int xPos, int yPos, int zPos,Scene_node* root_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(isSkybox, xPos, yPos, zPos,modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{

		m_firingCooldownReset = 25;
		m_firingCooldown = m_firingCooldownReset;
		m_speed = 0.1f;
		m_active = true;

		m_maxHealth = 30;
		m_currHealth = m_maxHealth;
		m_damageTaken = 0;
		m_damageTakenCooldownReset = 20;
		m_damageTakenCooldown = m_damageTakenCooldownReset;

		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetBelongsToEnemy(true);
		root_node->addChildNode(m_sceneNode);


		m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

		m_position = XMVectorSet(0.0, 0.0, 0.0, 0.0);

		m_moveLeftRight = 0.0f;
		m_moveBackForward = 0.0f;

		for (int x = 0; x < 10; x++)
		{
			bullets.push_back(new bullet(false, -100,-100,-100,false, root_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid,pDepthWrtieSkybox));
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
		}


	};

	void UpdateBullets(Scene_node* root_node);
	bool CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node);
	void UpdateEnemy(std::vector<bullet*> bullets, Scene_node* root_node, float x_lookAt, float y_lookAt);
	XMMATRIX UpdateMove(Scene_node* root_node);
	void MoveForward(Scene_node* root_node);
};
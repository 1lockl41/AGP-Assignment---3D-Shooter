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
	float m_firingCooldownReset;
	float m_firingCooldown;
	std::vector<bullet*> bullets;

	int m_currHealth;
	int m_maxHealth;

	int m_damageTaken;
	float m_damageTakenCooldown;
	float m_damageTakenCooldownReset;

public:

	enemy(Scene_node* root_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{

		m_firingCooldownReset = 400;
		m_firingCooldown = m_firingCooldownReset;
		m_moveSpeed = 0.005f;
		m_active = true;

		m_maxHealth = 30;
		m_currHealth = m_maxHealth;
		m_damageTaken = 0;
		m_damageTakenCooldownReset = 200;
		m_damageTakenCooldown = m_damageTakenCooldownReset;

		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetBelongsToEnemy(true);
		root_node->addChildNode(m_sceneNode);

		for (int x = 0; x < 10; x++)
		{
			bullets.push_back(new bullet(false, root_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext));
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
		}


	};

	void UpdateBullets(Scene_node* root_node);
	bool CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node);
	void UpdateEnemy(std::vector<bullet*> bullets, Scene_node* root_node);
};
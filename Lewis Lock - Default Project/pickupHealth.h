#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"


//Health pickup object, when the player collides with this object, will restore a set amount of health
class pickupHealth : public baseClass
{
private:

	int m_spawnX, m_spawnY, m_spawnZ;

	//How long between respawns
	float spawnTimer;
	float spawnTimerReset;


public:

	pickupHealth(int spawnX, int spawnY, int spawnZ, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(false, -100, -100, -100, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(0.5);
		root_node->addChildNode(m_sceneNode);

		m_spawnX = spawnX;
		m_spawnY = spawnY;
		m_spawnZ = spawnZ;

		spawnTimerReset = 2000;
		spawnTimer = spawnTimerReset;

		m_active = false;
	}

	void Spawn(); //Place on level
	void OnPickUp(); //Once picked up by player
	bool IsActive(); //Check if active
	void Update(); //Update spawn timer


};
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

class pickupShotgun : public baseClass
{
private:

	int m_spawnX, m_spawnY, m_spawnZ;
	float spawnTimer;
	float spawnTimerReset;


public:

	pickupShotgun(int spawnX, int spawnY, int spawnZ, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(false, -100, -100, -100, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(0.5);
		root_node->addChildNode(m_sceneNode);

		m_spawnX = spawnX;
		m_spawnY = spawnY;
		m_spawnZ = spawnZ;

		spawnTimerReset = 600;
		spawnTimer = spawnTimerReset;

		m_active = false;

	}

	void Spawn();
	void OnPickUp();
	bool IsActive();
	void Update();


};
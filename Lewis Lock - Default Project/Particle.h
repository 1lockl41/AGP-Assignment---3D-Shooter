#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

class Particle : public baseClass
{
private:

	int lifeTime;
	int lifeTimeReset;

	XMVECTOR m_dir;



public:

	Particle(float xPos, float yPos, float zPos, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(false, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(0.25);
		root_node->addChildNode(m_sceneNode);
		m_speed = 0.5;
		m_active = false;

		lifeTimeReset = 50;

		m_dir = XMVectorSet(0.0, 0.0, 0.0, 0.0);
	};

	void Move();

	bool isActive();
	void SetActive(bool active, float x_lookAt, float y_lookAt, float z_lookAt);

};
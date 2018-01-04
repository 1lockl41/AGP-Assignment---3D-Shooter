#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

//Particle class, a simple object which has no collision and will move in a set direction until their lifetime is over.
//Managed by a particle generator which they belong to.
class Particle : public baseClass
{
private:
	
	//The life time of the particle, once this is over it will become inactive and return to the pool
	int lifeTime;
	int lifeTimeReset;

	//Direction to move in
	XMVECTOR m_dir;

public:

	Particle(float xPos, float yPos, float zPos, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(false, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(0.25);
		root_node->addChildNode(m_sceneNode);
		m_speed = 0.04;
		m_active = false;

		lifeTimeReset = 50;

		m_dir = XMVectorSet(0.0, 0.0, 0.0, 0.0);
	};

	//Move based on direction
	void Move(double deltaTime);

	//Check if active
	bool isActive();

	//Spawn particle, setting active, position, and diection to move in
	void SetActive(bool active, float x_lookAt, float y_lookAt, float z_lookAt);

};
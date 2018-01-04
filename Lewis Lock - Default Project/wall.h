#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

//Wall object class. These blocks act as walls throughout the level, preventing actors from moving through them.
//The level uses two types, normal walls and removable walls. Removable walls are deactivated once triggered to allow movement.
class wall : public baseClass
{
private:
	float m_xSpawn;
	float m_ySpawn;
	float m_zSpawn;

public:

	wall(bool isSkybox, float xPos, float yPos, float zPos, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(isSkybox, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(4);
		root_node->addChildNode(m_sceneNode);
		m_active = true;

		m_xSpawn = xPos;
		m_ySpawn = yPos;
		m_zSpawn = zPos;

	};

	void SetActive(bool active); //Set wall to active or inactive. Used to deactivate removable walls.
	void Update(); //Check whether wall is active or inactive. Used to deactivate removable walls.
};
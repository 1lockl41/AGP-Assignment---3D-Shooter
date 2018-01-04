#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

//Pushable block object, can be moved around by the player when they move into it
class pushableBlock : public baseClass
{
private:
	XMVECTOR m_dir;


public:

	pushableBlock(int spawnX, int spawnY, int spawnZ, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(false, spawnX, spawnY, spawnZ, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(2);
		root_node->addChildNode(m_sceneNode);

		m_speed = 0.2f;

		m_dir = XMVectorSet(0.0, 0.0, 0.0, 0.0);
	}

	void MoveAwayFrom(float x_lookAt, float z_lookAt, Scene_node* root_node); //Move in an opposite direction to collider, usually player
	bool CheckIfTriggered(float zPos); //Check if has moved in a position which activates the trigger block

};
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

class wall : public baseClass
{
private:




public:

	wall(float xPos, float yPos, float zPos, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{
		setXPos(xPos);
		setYPos(yPos);
		setZPos(zPos);

		m_sceneNode->SetModel(m_model);

		root_node->addChildNode(m_sceneNode);

	};





};
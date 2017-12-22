#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"
#include "scene_node.h"

class baseClass
{
protected:

	Model* m_model;
	float m_xPos, m_yPos, m_zPos;
	float m_xAngle, m_yAngle, m_zAngle;
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11SamplerState* m_pSampler;
	Scene_node* m_sceneNode;
	bool m_active;
	float m_speed;

public:
	baseClass(bool isSkybox, int xPos, int yPos, int zPos, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	Model* getModel();
	Scene_node* getSceneNode();

	void setXPos(float x);
	void setYPos(float y);
	void setZPos(float z);
	void setScale(float scale);

	float getXPos();
	float getYPos();
	float getZPos();

	void LookAt_XZ(float xPos, float zPos);
};
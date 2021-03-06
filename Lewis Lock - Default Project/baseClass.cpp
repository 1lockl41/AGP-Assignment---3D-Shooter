#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"


baseClass::baseClass(bool isSkybox, int xPos, int yPos, int zPos, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox)
{

	m_model = new Model(pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox); //set model
	m_model->SetIsSkybox(isSkybox); //sets if this model is a skybox or not, inverts culling
	m_model->LoadObjModel(modelFilename); //Load model

	//Set the scene node which this object belongs to
	m_sceneNode = new Scene_node();

	//Initialise sampler
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

	//Create texture
	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, textureFilename, NULL, NULL, &m_pTexture, NULL);
	
	//Set object's texture and sampler
	m_model->SetTexture(m_pTexture);
	m_model->SetSampler(m_pSampler);

	//Set position
	setXPos(xPos);
	setYPos(yPos);
	setZPos(zPos);
}

//Return object's model
Model* baseClass::getModel()
{
	return m_model;
}

void baseClass::setXPos(float x)
{
	m_xPos = x;
	m_sceneNode->SetXPos(m_xPos);
}

void baseClass::setYPos(float y)
{
	m_yPos = y;
	m_sceneNode->SetYPos(m_yPos);
}

void baseClass::setZPos(float z)
{
	m_zPos = z;
	m_sceneNode->SetZPos(m_zPos);
}

float baseClass::getXPos()
{
	return m_xPos;
}

float baseClass::getYPos()
{
	return m_yPos;
}

float baseClass::getZPos()
{
	return m_zPos;
}

void baseClass::setScale(float scale)
{
	m_sceneNode->SetScale(scale);
}

Scene_node* baseClass::getSceneNode()
{
	return m_sceneNode;
}

//Set the models Y angle to face a position
void baseClass::LookAt_XZ(float xPos, float zPos)
{
	m_yAngle = atan2((xPos - m_xPos), (zPos - m_zPos))  * (180 / XM_PI); //calculate new angle

	//set new Y angle
	m_sceneNode->SetYangle(m_yAngle); 
	m_model->SetYangle(m_yAngle);
}

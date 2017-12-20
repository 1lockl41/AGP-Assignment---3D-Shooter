#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"


baseClass::baseClass(char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext)
{

	m_model = new Model(pD3DDevice, pImmediateContext);
	m_model->LoadObjModel(modelFilename);

	m_sceneNode = new Scene_node();

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

	D3DX11CreateShaderResourceViewFromFile(pD3DDevice, textureFilename, NULL, NULL, &m_pTexture, NULL);
	

	m_model->SetTexture(m_pTexture);
	m_model->SetSampler(m_pSampler);

}

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
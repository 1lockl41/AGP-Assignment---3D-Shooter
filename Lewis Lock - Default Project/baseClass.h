#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"

class baseClass
{
protected:

	Model* m_model;
	float m_xPos, m_yPos, m_zPos;
	ID3D11ShaderResourceView* m_pTexture;
	ID3D11SamplerState* m_pSampler;

public:
	baseClass(char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext);
	Model* getModel();

};
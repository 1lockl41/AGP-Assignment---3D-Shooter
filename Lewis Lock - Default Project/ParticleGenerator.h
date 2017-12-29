#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Particle.h"

class ParticleGenerator
{
private:

	float xPos;
	float yPos;
	float zPos;
	
	std::vector<Particle*> particles;



public:
	ParticleGenerator(int numberOfParticles, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	void SpawnParticles();
	void UpdateParticles(double deltaTime);
	void SetPosition(float x, float y, float z);



};
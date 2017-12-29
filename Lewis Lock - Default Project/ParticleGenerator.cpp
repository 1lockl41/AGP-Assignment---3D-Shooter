#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "ParticleGenerator.h"
#include <cstdlib>
#include <ctime>


ParticleGenerator::ParticleGenerator(int numberOfParticles, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox)
{
	srand(static_cast <unsigned> (time(0)));

	for (int i = 0; i < numberOfParticles; i++)
	{
		particles.push_back(new Particle(false, -100, -100, root_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox));

	}
}

void ParticleGenerator::SpawnParticles()
{
	for (int i = 0; i < particles.size(); i++)
	{
		if (!particles[i]->isActive())
		{
			float minRandom = -100;
			float maxRandom = 100;

			float x_lookAt = minRandom + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(maxRandom - minRandom)));
			float y_lookAt = minRandom + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRandom - minRandom)));
			float z_lookAt = minRandom + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRandom - minRandom)));

			particles[i]->setXPos(xPos);
			particles[i]->setYPos(yPos);
			particles[i]->setZPos(zPos);
			particles[i]->SetActive(true, x_lookAt, y_lookAt, z_lookAt);
		}
	}

}

void ParticleGenerator::UpdateParticles(double deltaTime)
{
	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i]->isActive())
		{

			particles[i]->Move(deltaTime);
		}

	}

}

void ParticleGenerator::SetPosition(float x, float y, float z)
{
	xPos = x;
	yPos = y;
	zPos = z;
}
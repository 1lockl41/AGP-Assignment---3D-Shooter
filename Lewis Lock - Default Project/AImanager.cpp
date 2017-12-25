#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "AImanager.h"

AImanager::AImanager(int numberOfEnemies, Scene_node* actors_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWriteSkybox)
{
	m_numberOfEnemies = numberOfEnemies;

	m_SpawnPoint1 = XMVectorSet(6.0, 0.0, 6.0, 0.0);
	m_SpawnPoint2 = XMVectorSet(6.0, 0.0, 32.0, 0.0);
	m_SpawnPoint3 = XMVectorSet(32.0, 0.0, 6.0, 0.0);
	m_SpawnPoint4 = XMVectorSet(32.0, 0.0, 32.0, 0.0);


	XMVECTOR currentSpawnPoint = XMVectorZero();
	int currentSpawnLoop = 0;

	for (int i = 0; i < numberOfEnemies; i++)
	{
		if (currentSpawnLoop == 0)
		{
			currentSpawnPoint = m_SpawnPoint1;
			currentSpawnLoop = 1;
		}
		else if (currentSpawnLoop == 1)
		{
			currentSpawnPoint = m_SpawnPoint2;
			currentSpawnLoop = 2;
		}
		else if (currentSpawnLoop == 2)
		{
			currentSpawnPoint = m_SpawnPoint3;
			currentSpawnLoop = 3;
		}
		else if (currentSpawnLoop == 3)
		{
			currentSpawnPoint = m_SpawnPoint4;
			currentSpawnLoop = 0;
		}


		enemies.push_back(new enemy(false, XMVectorGetX(currentSpawnPoint), XMVectorGetY(currentSpawnPoint), XMVectorGetZ(currentSpawnPoint), actors_node, bulletModelFileName, bulletTextureFileName, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWriteSkybox));
	}

}

std::vector<enemy*> AImanager::GetEnemies()
{
	return enemies;
}

std::vector<bullet*> AImanager::GetAllBullets()
{
	std::vector<bullet*> tempAllBullets;

	for (int i = 0; i < enemies.size(); i++)
	{
		std::vector<bullet*> tempBullets = enemies[i]->GetBullets();
		for (int j = 0; j < tempBullets.size(); j++)
		{
			tempAllBullets.push_back(tempBullets[j]);
		}

	}

	return tempAllBullets;
}

void AImanager::UpdateAllEnemies(std::vector<bullet*> bullets, Scene_node* actors_node, float x_lookAt, float y_lookAt, Scene_node* walls_node, player* player1)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->UpdateBullets(walls_node);
		enemies[i]->UpdateEnemy(bullets, actors_node, x_lookAt, y_lookAt, player1);

	}
}
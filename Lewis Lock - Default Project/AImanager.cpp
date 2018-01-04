#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "AImanager.h"

AImanager::AImanager(int numberOfEnemies, Scene_node* actors_node, Scene_node* particles_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWriteSkybox)
{
	m_numberOfEnemies = numberOfEnemies;

	//The current spawn point to use
	currentSpawnLoop = 0;

	//Set spawn point positions
	m_SpawnPoint1 = XMVectorSet(12.0, 0.0, 64.0, 0.0);
	m_SpawnPoint2 = XMVectorSet(12.0, 0.0, 12.0, 0.0);
	m_SpawnPoint3 = XMVectorSet(64.0, 0.0, 12.0, 0.0);
	m_SpawnPoint4 = XMVectorSet(72.0, 0.0, 64.0, 0.0);

	//Set spawn timers
	spawnEnemyCooldownReset = 350;
	spawnEnemyCooldown = spawnEnemyCooldownReset;

	//Initialise enemies to vector
	for (int i = 0; i < numberOfEnemies; i++)
	{
		enemies.push_back(new enemy(false, -100, -100, -100, actors_node, particles_node, bulletModelFileName, bulletTextureFileName, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWriteSkybox));
	}

}

//Returns all enemies handled by this AI manager
std::vector<enemy*> AImanager::GetEnemies()
{
	return enemies;
}

//Returns all bullets belonging to all enemies
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

//Update all enemies and their bullets
void AImanager::UpdateAllEnemies(std::vector<bullet*> bullets, Scene_node* actors_node, float x_lookAt, float y_lookAt, Scene_node* walls_node, player* player1, double deltaTime)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->UpdateBullets(walls_node, deltaTime);
		enemies[i]->UpdateEnemy(bullets, actors_node, x_lookAt, y_lookAt, player1, deltaTime);
	}
}

//Check if an enemy can be spawned, in which case spawn them at the current spawn point
void AImanager::CheckSpawnEnemies(double deltaTime)
{
	//Reduce cooldown timer, uses delta time to ensure its the same time across different frame rates
	spawnEnemyCooldown -= (deltaTime*0.05);
	if (spawnEnemyCooldown < 0)
		spawnEnemyCooldown = 0;

	//If an enemy can spawn
	if (spawnEnemyCooldown == 0)
	{
		bool hasSpawned = false; //set to true once an enemy has spawned, stopping an addition one from spawning

		//Initialise current spawn point
		XMVECTOR currentSpawnPoint = XMVectorZero();
	
		//Get the correct spawn location depending on the current spawn loop
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

		//Get the first enemy that is not active in the pool and place them at the new location, activating them
		for (int i = 0; i < enemies.size(); i++)
		{
			if (!enemies[i]->IsActive() && !hasSpawned)
			{
				enemies[i]->SetActive(XMVectorGetX(currentSpawnPoint), XMVectorGetY(currentSpawnPoint), XMVectorGetZ(currentSpawnPoint));
				hasSpawned = true;
			}

		}

		//Gradually reduce the reset time, so enemies spawn quicker as the game progresses
		spawnEnemyCooldownReset -= 15;
		if (spawnEnemyCooldownReset < 100)
			spawnEnemyCooldownReset = 100;

		//Reset spawn timer
		spawnEnemyCooldown = spawnEnemyCooldownReset;
	}
}

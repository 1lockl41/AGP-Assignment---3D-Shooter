#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "enemy.h"
#include "bullet.h"

//The AI manager class stores all the enemies in a pool, activating them when required. Also handles their AI and spawns them.
class AImanager
{
private:

	//Store the possible spawn points on map, current all corners
	XMVECTOR m_SpawnPoint1;
	XMVECTOR m_SpawnPoint2;
	XMVECTOR m_SpawnPoint3;
	XMVECTOR m_SpawnPoint4;

	int m_numberOfEnemies; //number of enemies initialised
	std::vector<enemy*> enemies; //vector which stores all the enemy objects

	float spawnEnemyCooldown; //curent enemy spawn timer
	float spawnEnemyCooldownReset; //the value to reset the spawn timer to
	int currentSpawnLoop; //the current spawn point to use



public:
	AImanager(int numberOfEnemies, Scene_node* actors_node, Scene_node* particles_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWriteSkybox);
	std::vector<enemy*> GetEnemies(); //returns all enemies
	std::vector<bullet*> GetAllBullets(); //returns all bullets which belong to enemies

	//Update all enemy AI, moving them and checking collisions. Also updates their bullets
	void UpdateAllEnemies(std::vector<bullet*> bullets, Scene_node* actors_node, float x_lookAt, float y_lookAt, Scene_node* walls_node, player* player1, double deltaTime);

	//Check if and enemy can be spawned
	void CheckSpawnEnemies(double deltaTime);
};
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "enemy.h"
#include "bullet.h"

class AImanager
{
private:

	XMVECTOR m_SpawnPoint1;
	XMVECTOR m_SpawnPoint2;
	XMVECTOR m_SpawnPoint3;
	XMVECTOR m_SpawnPoint4;

	int m_numberOfEnemies;
	std::vector<enemy*> enemies;

	float spawnEnemyCooldown;
	float spawnEnemyCooldownReset;
	int currentSpawnLoop;



public:
	AImanager(int numberOfEnemies, Scene_node* actors_node, Scene_node* particles_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWriteSkybox);
	std::vector<enemy*> GetEnemies();
	std::vector<bullet*> GetAllBullets();

	void UpdateAllEnemies(std::vector<bullet*> bullets, Scene_node* actors_node, float x_lookAt, float y_lookAt, Scene_node* walls_node, player* player1, double deltaTime);

	void CheckSpawnEnemies(double deltaTime);
};
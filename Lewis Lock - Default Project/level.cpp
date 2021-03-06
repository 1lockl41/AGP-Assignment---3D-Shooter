#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "level.h"

level::level(bool isSkybox, int levelSizeX, int levelSizeZ, Scene_node* root_node, Scene_node* floor_node, char* modelFilename, char* textureFilename, char* floor_textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox)
{
	m_level_node = new Scene_node();
	root_node->addChildNode(m_level_node); 

	m_levelSizeX = levelSizeX;
	m_levelSizeZ = levelSizeZ;

	//loop through and initialise level
	for (int i = 0; i < m_levelSizeX; i++)
	{
		std::vector<wall*> vec; //temp vector, will store z axis of blocks for floor
		std::vector<int> vec2; //temp vector, will store z axis of blocks for walls
		for (int j = 0; j < m_levelSizeZ; j++)
		{
			//add floor wall block to z axis 
			vec.push_back(new wall(isSkybox,i * 8, -8, j * 8, floor_node, modelFilename, floor_textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox));
			vec2.push_back(0);
		}
		m_levelFloorVector2D.push_back(vec); //push back x axis of vector with the temp z axis vector
		m_levelWallsVector2D.push_back(vec2);
	}

	InitialiseLevelWalls();
	SetupLevelWalls(isSkybox, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox);
}

//Set up the positions and layout of level. 
void level::InitialiseLevelWalls()
{
	////Automatically set outisde walls based on size of level/////
	for (int i = 0; i < m_levelSizeX; i++)
	{
		for (int j = m_levelSizeZ - 1; j < m_levelSizeZ; j++)
		{
			m_levelWallsVector2D[i][j] = 1;
		}
	}

	for (int i = 0; i < m_levelSizeX; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			m_levelWallsVector2D[i][j] = 1;
		}
	}

	for (int i = m_levelSizeX-1; i < m_levelSizeX; i++)
	{
		for (int j = 0; j < m_levelSizeZ; j++)
		{
			m_levelWallsVector2D[i][j] = 1;
		}
	}

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < m_levelSizeZ; j++)
		{
			m_levelWallsVector2D[i][j] = 1;
		}
	}
	//////////////////////////////////////////////////////////////

	//Specically set positions of walls
	m_levelWallsVector2D[6][6] = 1;
	m_levelWallsVector2D[6][7] = 1;
	m_levelWallsVector2D[6][8] = 1;

	m_levelWallsVector2D[5][6] = 1;
	m_levelWallsVector2D[7][6] = 1;

	m_levelWallsVector2D[6][9] = 2;
	m_levelWallsVector2D[6][10] = 2;
	m_levelWallsVector2D[7][6] = 2;
	m_levelWallsVector2D[8][6] = 2;
	m_levelWallsVector2D[9][6] = 2;
	m_levelWallsVector2D[10][6] = 2;
}

//Create walls blocks based on level data.
void level::SetupLevelWalls(bool isSkybox, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox)
{
	//Loop through walls vector, placing the specified walls at their correct position
	for (int i = 0; i < m_levelSizeX; i++)
	{
		for (int j = 0; j < m_levelSizeZ; j++)
		{
			if (m_levelWallsVector2D[i][j] == 1)
			{
				wall* tempWall = new wall(isSkybox, i * 8, 0, j * 8, m_level_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox);
				tempWall->getSceneNode()->SetScale(4);
			}
			else if (m_levelWallsVector2D[i][j] == 2)
			{
				wall* tempWall = new wall(isSkybox, i * 8, -4, j * 8, m_level_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox);
				tempWall->getSceneNode()->SetScale(4);
				m_removableWalls.push_back(tempWall);
			}

		}
	}
}

void level::UpdateRemoveableWalls()
{
	for (int i = 0; i < m_removableWalls.size(); i++)
	{
		m_removableWalls[i]->Update();
	}
}

void level::DeactivateRemoveableWalls()
{
	for (int i = 0; i < m_removableWalls.size(); i++)
	{
		m_removableWalls[i]->SetActive(false);
		m_removableWalls[i]->setXPos(100);
	}
}
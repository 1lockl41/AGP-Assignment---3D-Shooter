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


void level::InitialiseLevelWalls()
{
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


	m_levelWallsVector2D[6][6] = 1;
	m_levelWallsVector2D[6][7] = 1;
	m_levelWallsVector2D[6][8] = 1;

	m_levelWallsVector2D[5][6] = 1;
	m_levelWallsVector2D[7][6] = 1;
}

void level::SetupLevelWalls(bool isSkybox, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox)
{

	for (int i = 0; i < m_levelSizeX; i++)
	{
		for (int j = 0; j < m_levelSizeZ; j++)
		{
			if (m_levelWallsVector2D[i][j] == 1)
			{
				wall* tempWall = new wall(isSkybox, i * 8, 0, j * 8, m_level_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox);
				//wall* tempWall2 = new wall(isSkybox, i * 2, 0, j * 2, m_level_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox);
				tempWall->getSceneNode()->SetScale(4);
			}

		}
	}


}
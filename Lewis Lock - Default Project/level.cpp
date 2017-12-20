#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "level.h"

level::level(int levelSizeX, int levelSizeZ, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext)
{
	m_level_node = new Scene_node();
	root_node->addChildNode(m_level_node); 

	//loop through and initialise level
	for (int i = 0; i < levelSizeX; i++)
	{
		std::vector<wall*> vec; //temp vector, will store z axis of blocks
		for (int j = 0; j < levelSizeZ; j++)
		{
			//add wall block to z axis 
			vec.push_back(new wall(i * 2, -4, j * 2, m_level_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext));
		}
		m_levelFloorVector2D.push_back(vec); //push back x axis of vector with the temp z axis vector
	}
}

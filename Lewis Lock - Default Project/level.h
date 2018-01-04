#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "wall.h"

//The level class, stores details about the levels size and wall placement. Generates the level based on this. Can be easily changed to allow quick edits to level size, and walls within the level.
//All outside walls are automatically generated based on level size, other walls must be specifically placed
class level
{
private:
	int m_levelSizeX; //size of level on X axis
	int m_levelSizeZ; //size of level on Z axis
	Scene_node* m_level_node; //The scene node which walls are attatched to
	std::vector<std::vector<wall*>> m_levelFloorVector2D; //contains size and width of game's floor, composed of wall blocks
	std::vector<std::vector<int>> m_levelWallsVector2D; //contains a map of integers, 0 indicates and empty position, 1 represents a wall, and 2 represents a walls which can be removed
	std::vector<wall*> m_removableWalls; //A vector containing all of the removable walls which are created in the above vector

public:

	level(bool isSkybox, int levelSizeX, int levelSizeZ, Scene_node* root_node, Scene_node* floor_node, char* modelFilename, char* textureFilename, char* floor_textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);

	//Set up the positions and layout of level. 
	void InitialiseLevelWalls(); 

	//Create walls blocks based on level data.
	void SetupLevelWalls(bool isSkybox, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	
	//Check if removable walls should be removed from level
	void UpdateRemoveableWalls();
	
	//Remove removable walls from level
	void DeactivateRemoveableWalls();
};
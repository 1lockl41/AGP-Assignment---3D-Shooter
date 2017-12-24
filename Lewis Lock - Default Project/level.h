#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "wall.h"

class level
{
private:

	int m_levelSizeX;
	int m_levelSizeZ;
	Scene_node* m_level_node;
	std::vector<std::vector<wall*>> m_levelFloorVector2D; //contains size and width of game's floor, composed of wall blocks
	std::vector<std::vector<int>> m_levelWallsVector2D;

public:

	level(bool isSkybox, int levelSizeX, int levelSizeZ, Scene_node* root_node, Scene_node* floor_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	void InitialiseLevelWalls();
	void SetupLevelWalls(bool isSkybox, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);

};
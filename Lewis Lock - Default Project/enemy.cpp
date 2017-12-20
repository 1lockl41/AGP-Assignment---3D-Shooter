#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "enemy.h"


void enemy::UpdateBullets(Scene_node* root_node)
{
	for (int x = 0; x < bullets.size(); x++)
	{
		bullets[x]->UpdateBullet(root_node);
	}
}

bool enemy::CheckCollisions(Scene_node* root_node)
{
	
	if (m_sceneNode->check_collision(root_node, m_sceneNode, false))
	{
		return true;
	}
	else
	{
		return false;
	}
	


}
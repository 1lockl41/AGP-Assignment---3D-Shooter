#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "enemy.h"


void enemy::UpdateBullets()
{
	for (int x = 0; x < 10; x++)
	{
		bullets[x]->UpdateBullet();
	}
}
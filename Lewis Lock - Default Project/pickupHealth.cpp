#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "pickupHealth.h"


void pickupHealth::OnPickUp()
{
	m_active = false;
	setXPos(-100);
	setYPos(-100);
	setZPos(-100);

}

void pickupHealth::Spawn()
{
	setXPos(m_spawnX);
	setYPos(m_spawnY);
	setZPos(m_spawnZ);
	m_active = true;

}

bool pickupHealth::IsActive()
{
	return m_active;
}

void pickupHealth::Update()
{
	if (!m_active)
	{
		spawnTimer--;
		if (spawnTimer < 0)
			spawnTimer = 0;

		if (spawnTimer == 0)
		{
			Spawn();
			spawnTimer = spawnTimerReset;
		}
	}

}
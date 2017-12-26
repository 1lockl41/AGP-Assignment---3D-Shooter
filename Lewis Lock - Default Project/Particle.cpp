#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Particle.h"

void Particle::Move()
{
	lifeTime--;
	if (lifeTime < 0)
		lifeTime = 0;

	if (lifeTime == 0)
	{

		m_active = false;
		setXPos(-100);
		setYPos(-100);
		setZPos(-100);
	}
	else
	{
		setXPos(getXPos() + (XMVectorGetX(m_dir)*m_speed));
		setYPos(getYPos() + (XMVectorGetY(m_dir)*m_speed));
		setZPos(getZPos() + (XMVectorGetZ(m_dir)*m_speed));
	}
}

bool Particle::isActive()
{
	return m_active;
}

void Particle::SetActive(bool active,float x_lookAt, float y_lookAt, float z_lookAt)
{
	m_dir = XMVectorSet(x_lookAt - m_xPos, y_lookAt - m_yPos, z_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);

	m_active = active;
	lifeTime = lifeTimeReset;
}
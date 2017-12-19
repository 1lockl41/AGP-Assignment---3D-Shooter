#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "bullet.h"

void bullet::moveForward()
{
	m_moveBackForward += m_speed;

}

XMMATRIX bullet::UpdateBulletRotation()
{

	m_bulletRotationMatrix = XMMatrixRotationRollPitchYaw(m_dx, m_dz, 0);
	m_lookat = XMVector3TransformCoord(m_defaultForward, m_bulletRotationMatrix);
	m_lookat = XMVector3Normalize(m_lookat);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_dz);

	m_bulletRight = XMVector3TransformCoord(m_defaultRight, RotateYTempMatrix);
	m_up = XMVector3TransformCoord(m_up, RotateYTempMatrix);
	m_bulletForward = XMVector3TransformCoord(m_defaultForward, RotateYTempMatrix);

	m_position += m_moveLeftRight*m_bulletRight;
	m_position += m_moveBackForward*m_bulletForward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	m_lookat = m_position + m_lookat;

	setXPos(getXPos() + XMVectorGetX(m_position));
	setYPos(getYPos() + XMVectorGetY(m_position));
	setZPos(getZPos() + XMVectorGetZ(m_position));

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

void bullet::UpdateBullet()
{
	if (m_active)
	{
		m_activeTime--;
		if (m_activeTime < 0)
			m_activeTime = 0;

		if (m_activeTime == 0)
		{
			m_active = false;
			m_activeTime = m_activeTimeReset;
			setXPos(-100);
			setYPos(-100);
			setZPos(-100);
		}

		moveForward();
		UpdateBulletRotation();
	}
	else
	{
		setXPos(-100);
		setYPos(-100);
		setZPos(-100);
	}
}

void bullet::SetActive(float xPos, float yPos, float zPos, float dx, float dz)
{
	m_active = true; 
	setXPos(xPos);
	setYPos(yPos);
	setZPos(zPos);
	m_position = XMVectorSet(0.0, 0.0, 0.0, 0.0);

	m_dx = dx;
	m_dz = dz;
}

bool bullet::IsActive()
{
	return m_active;
}
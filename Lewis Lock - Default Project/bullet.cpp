#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "bullet.h"

void bullet::moveForward(double deltaTime)
{
	m_moveBackForward += (deltaTime*m_speed);
}

void bullet::UpdateBulletRotation()
{
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_zAngle);

	m_bulletRight = XMVector3TransformCoord(m_defaultRight, RotateYTempMatrix);
	m_bulletForward = XMVector3TransformCoord(m_defaultForward, RotateYTempMatrix);

	m_position += m_moveLeftRight*m_bulletRight;
	m_position += m_moveBackForward*m_bulletForward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	setXPos(getXPos() + XMVectorGetX(m_position));
	setYPos(getYPos() + XMVectorGetY(m_position));
	setZPos(getZPos() + XMVectorGetZ(m_position));
}

void bullet::SetDirection(float x_lookAt, float y_lookAt)
{
	m_dir = XMVectorSet(x_lookAt - m_xPos, 0.0, y_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);
}

void bullet::MoveTowards(double deltaTime)
{
	setXPos(m_xPos + (XMVectorGetX(m_dir)*(deltaTime*m_speed)));
	setZPos(m_zPos + (XMVectorGetZ(m_dir)*(deltaTime*m_speed)));
}

void bullet::UpdateBullet(Scene_node* root_node, double deltaTime)
{
	if (m_active)
	{
		m_activeTime -= (deltaTime*0.05);
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

		if (m_sceneNode->GetBelongsToPlayer())
		{
			moveForward(deltaTime);
			UpdateBulletRotation();
		}
		else
		{
			MoveTowards(deltaTime);
		}

		XMMATRIX identity = XMMatrixIdentity();
		root_node->UpdateCollisionTree(&identity, 1.0);

		if(m_sceneNode->check_collision(root_node, m_sceneNode) == true)
		{
			m_active = false;
			m_activeTime = m_activeTimeReset;
			setXPos(-100);
			setYPos(-100);
			setZPos(-100);
		}

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

	m_xAngle = dx;

	m_zAngle = dz;
}

void bullet::Deactivate()
{
	m_active = false;
	m_activeTime = m_activeTimeReset;
	setXPos(-100);
	setYPos(-100);
	setZPos(-100);
}

bool bullet::IsActive()
{
	return m_active;
}

int bullet::GetDamage()
{
	return m_damage;
}
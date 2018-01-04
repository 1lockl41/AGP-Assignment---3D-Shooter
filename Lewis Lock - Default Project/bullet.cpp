#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "bullet.h"

//Move the bullet foward, used by player to move based on camera direction, uses delta speed to ensure its the same speed across different frame rates
void bullet::moveForward(double deltaTime)
{
	m_moveBackForward += (deltaTime*m_speed);
}

//Set the direction the bullet should be moving when fired by player, based on angle of camera
void bullet::UpdateBulletRotation()
{
	//Create matrix based on z angle of player's camera
	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_zAngle);

	m_bulletRight = XMVector3TransformCoord(m_defaultRight, RotateYTempMatrix);
	m_bulletForward = XMVector3TransformCoord(m_defaultForward, RotateYTempMatrix);

	//calculate new position vector to move in, reset this value after movement
	m_position += m_moveLeftRight*m_bulletRight;
	m_position += m_moveBackForward*m_bulletForward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	//set new position
	setXPos(getXPos() + XMVectorGetX(m_position));
	setYPos(getYPos() + XMVectorGetY(m_position));
	setZPos(getZPos() + XMVectorGetZ(m_position));
}

//Set the direction to look at and move, used when fired by an enemy
void bullet::SetDirection(float x_lookAt, float y_lookAt)
{
	m_dir = XMVectorSet(x_lookAt - m_xPos, 0.0, y_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);
}

//Move bullet forward based on direction, used by enemies when fired as they have a set direction to use, unlike player's camera
void bullet::MoveTowards(double deltaTime)
{
	setXPos(m_xPos + (XMVectorGetX(m_dir)*(deltaTime*m_speed)));
	setZPos(m_zPos + (XMVectorGetZ(m_dir)*(deltaTime*m_speed)));
}

//Update bullet, moving it and checking collision
void bullet::UpdateBullet(Scene_node* root_node, double deltaTime)
{
	//If active, move and check collisions, otherwise, set to inactive position outside o map
	if (m_active)
	{
		//Reduce current life time
		m_activeTime -= (deltaTime*0.05);
		if (m_activeTime < 0)
			m_activeTime = 0;

		//If life time has ended, set inactive
		if (m_activeTime == 0)
		{
			m_active = false;
			m_activeTime = m_activeTimeReset;
			setXPos(-100);
			setYPos(-100);
			setZPos(-100);
		}

		//If this bullet belongs to the player, move based on camera rotation, otherwise move based on enemy's direction vector
		if (m_sceneNode->GetBelongsToPlayer())
		{
			moveForward(deltaTime);
			UpdateBulletRotation();
		}
		else
		{
			MoveTowards(deltaTime);
		}

		//Update collision tree
		XMMATRIX identity = XMMatrixIdentity();
		root_node->UpdateCollisionTree(&identity, 1.0);

		//If colliding with any object, deactivate
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

//Spawn bullet, setting active, position, and angle
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
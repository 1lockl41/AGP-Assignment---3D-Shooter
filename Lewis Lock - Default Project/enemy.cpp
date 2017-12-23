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

bool enemy::CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node)
{
	for (int i = 0; i < bullets.size(); i++)
	{
	
		//only check for collisions if both nodes contain a model
		if (this->getModel() && bullets[i]->getModel())
		{
			XMVECTOR v1 = this->getSceneNode()->GetWorldCentrePosition();
			XMVECTOR v2 = bullets[i]->getSceneNode()->GetWorldCentrePosition();
			XMVECTOR vdiff = v1 - v2;
	
			XMVECTOR a = XMVector3Length(vdiff);
			float x1 = XMVectorGetX(v1);
			float x2 = XMVectorGetX(v2);
			float y1 = XMVectorGetY(v1);
			float y2 = XMVectorGetY(v2);
			float z1 = XMVectorGetZ(v1);
			float z2 = XMVectorGetZ(v2);
	
			float dx = x1 - x2;
			float dy = y1 - y2;
			float dz = z1 - z2;
	
			//check bounding sphere collision
			if (sqrt(dx*dx + dy*dy + dz*dz) < (bullets[i]->getModel()->GetBoundingSphereRadius() * bullets[i]->getSceneNode()->GetWorldScale()) + (this->getModel()->GetBoundingSphereRadius() * this->getSceneNode()->GetWorldScale()))
			{
				m_damageTaken = bullets[i]->GetDamage();
				return true;
			}
		}
	}

	return false;

}

void enemy::UpdateEnemy(std::vector<bullet*> bullets, Scene_node* root_node, float x_lookAt, float y_lookAt)
{
	if (m_active)
	{
		m_damageTakenCooldown--;

		LookAt_XZ(x_lookAt, y_lookAt);
		MoveTowards(root_node,x_lookAt, y_lookAt);
		CheckFiring(x_lookAt, y_lookAt);

		if (CheckCollisionsBullets(bullets, root_node))
		{

			if (m_damageTakenCooldown <= 0)
			{
				m_currHealth -= m_damageTaken;
				m_damageTaken = 0;

				m_damageTakenCooldown = m_damageTakenCooldownReset;
			}

			if (m_currHealth <= 0)
			{
				m_active = false;
				setXPos(-100);
				setYPos(-100);
				setZPos(-100);
				m_damageTakenCooldown = m_damageTakenCooldownReset;
			}
		}
	}
	else
	{
		setXPos(-100);
		setYPos(-100);
		setZPos(-100);
	}

}

void enemy::MoveTowards(Scene_node* root_node, float x_lookAt, float y_lookAt)
{
	m_dir = XMVectorSet(x_lookAt - m_xPos, 0.0, y_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);

	setXPos(m_xPos + (XMVectorGetX(m_dir)*m_speed));
	setZPos(m_zPos + (XMVectorGetZ(m_dir)*m_speed));
}


void enemy::CheckFiring(float x_lookAt, float y_lookAt)
{
	m_firingCooldown--;
	if (m_firingCooldown < 0)
		m_firingCooldown = 0;


	bool loop = true;
	if (m_firingCooldown == 0)
	{
		for (int x = 0; x < bullets.size(); x++)
		{
			if (!bullets[x]->IsActive() && loop)
			{
				bullets[x]->SetActive(m_xPos, m_yPos, m_zPos, m_xAngle, m_zAngle);
				bullets[x]->SetDirection(x_lookAt, y_lookAt);
				m_firingCooldown = m_firingCooldownReset;
				loop = false;
			}
		}
	}

}
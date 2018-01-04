#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "enemy.h"

//Updates all bullets belonging to this enemy, moves them and checks their collision
void enemy::UpdateBullets(Scene_node* root_node, double deltaTime)
{
	for (int x = 0; x < bullets.size(); x++)
	{
		bullets[x]->UpdateBullet(root_node, deltaTime);
	}
}

//Check if this enemy has been hit by a player's bullet 
bool enemy::CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node)
{
	//Loop through bullets array (usually the players)
	for (int i = 0; i < bullets.size(); i++)
	{
		//If this bullet is active, check if it is colliding with this enemy
		if (bullets[i]->IsActive())
		{
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

				//check bounding sphere collision, if collidiing then deativate the bullet and remove health from player, based on the damage of the bullet
				if (sqrt(dx*dx + dy*dy + dz*dz) < (bullets[i]->getModel()->GetBoundingSphereRadius() * bullets[i]->getSceneNode()->GetWorldScale()) + (this->getModel()->GetBoundingSphereRadius() * this->getSceneNode()->GetWorldScale()))
				{
					m_damageTaken = bullets[i]->GetDamage();
					bullets[i]->Deactivate();
					return true;
				}
			}
		}
	}

	return false;
}

//Update this enemy, moving them and checking collision
void enemy::UpdateEnemy(std::vector<bullet*> bullets, Scene_node* root_node, float x_lookAt, float y_lookAt, player* player1, double deltaTime)
{
	//Move any particles which are active, regardless if the enemy is active or not
	//Allows movement of particles after enemy's death
	particleGenerator->UpdateParticles(deltaTime);

	//If the enemy is active, move them.
	//Else, set them to their inactive position outside of the level
	if (m_active)
	{
		//Update the damage taken cooldown, based on delta time, allowing it to remain consistent across different frame rates
		m_damageTakenCooldown -= (deltaTime*0.05);

		//Get distance between player and enemy
		XMVECTOR v1 = this->getSceneNode()->GetWorldCentrePosition();
		XMVECTOR v2 = player1->getSceneNode()->GetWorldCentrePosition();

		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		//if player is within range, move towards them
		if (sqrt(dx*dx + dy*dy + dz*dz) < m_sightRange)
		{
			LookAt_XZ(x_lookAt, y_lookAt); //Face the player
			MoveTowards(root_node, x_lookAt, y_lookAt, deltaTime); //Move towards the player
			CheckFiring(x_lookAt, y_lookAt, deltaTime); //Check if they can fire, shooting towards the player if they can
		}

		//Check collisions
		if (m_sceneNode->check_collision(root_node, m_sceneNode))
		{
			//If colliding with a player's bullet
			if (CheckCollisionsBullets(bullets, root_node))
			{
				//If the enemy can current take damage, remove health based on damage of colliding bullet and reset damage cooldown
				if (m_damageTakenCooldown <= 0)
				{
					m_currHealth -= m_damageTaken;
					m_damageTaken = 0;

					m_damageTakenCooldown = m_damageTakenCooldownReset;
				}

				//If enemy has died, create a particle generator and set them to inactive, outside of game level
				if (m_currHealth <= 0)
				{
					particleGenerator->SetPosition(m_xPos, m_yPos, m_zPos);
					particleGenerator->SpawnParticles();

					m_active = false;
					setXPos(-100);
					setYPos(-100);
					setZPos(-100);
					m_damageTakenCooldown = m_damageTakenCooldownReset;
					player1->AddPlayerScore(5);
				}
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

//The point to move towards, will set what direction they face and where they move to
void enemy::MoveTowards(Scene_node* root_node, float x_lookAt, float z_lookAt, double deltaTime)
{
	//Caclulate new direction
	m_dir = XMVectorSet(x_lookAt - m_xPos, 0.0, z_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);

	//Move, checking collision
	m_sceneNode->IncSetX(m_xPos + (XMVectorGetX(m_dir)*(deltaTime*m_speed)), root_node);
	m_sceneNode->IncSetZ(m_zPos + (XMVectorGetZ(m_dir)*(deltaTime*m_speed)), root_node);

	//Set new position
	setXPos(m_sceneNode->GetXPos());
	setZPos(m_sceneNode->GetZPos());
}

//Check whether the enemy can fire a new bullet
void enemy::CheckFiring(float x_lookAt, float y_lookAt, double deltaTime)
{
	//Reduce firing cooldown, based on delta time
	m_firingCooldown-=(deltaTime*0.05);
	if (m_firingCooldown < 0)
		m_firingCooldown = 0;


	bool loop = true; //Set to false once a bullet has been fired, preventing more than one from being shot at once

	//If enemy can fire a bullet, spawn a new one moving in their current direction
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

//Get all bullets belonging to this enemy
std::vector<bullet*> enemy::GetBullets()
{
	return bullets;
}

//Spawn enemy, setting them as active and their position
void enemy::SetActive(float xPos, float yPos, float zPos)
{
	m_active = true;
	setXPos(xPos);
	setYPos(yPos);
	setZPos(zPos);
	m_currHealth = m_maxHealth;
}

//check if active
bool enemy::IsActive()
{
	return m_active;
}
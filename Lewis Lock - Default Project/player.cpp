#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "player.h"

Camera* player::getCamera()
{
	return m_playerCamera;
}

void player::RotateCamera(InputManager* inputManager)
{
	///////Camera facing movement with arrow keys//////
	if (inputManager->IsKeyPressed(DIK_DOWN))
	{
		getCamera()->Pitch(0.0002f);
	}
	if (inputManager->IsKeyPressed(DIK_UP))
	{
		getCamera()->Pitch(-0.0002f);
	}
	if (inputManager->IsKeyPressed(DIK_LEFT))
	{
		getCamera()->Yaw(-0.0002f);
	}
	if (inputManager->IsKeyPressed(DIK_RIGHT))
	{
		getCamera()->Yaw(0.0002f);
	}
	////////////////////////////////////////////

	///////Camera facing movement with mouse///////////
	if ((inputManager->GetMouseState().lX != inputManager->GetLastMouseState().lX) || (inputManager->GetMouseState().lY != inputManager->GetLastMouseState().lY))
	{
		getCamera()->Yaw(inputManager->GetMouseState().lX * 0.001f);
		//getCamera()->Pitch(inputManager->GetMouseState().lY * 0.001f);

		inputManager->SetLastMouseState(inputManager->GetMouseState());
	}
	//////////////////////////////////////////////////
}

void player::MoveCamera(InputManager* inputManager, Scene_node* root_node, double deltaTime)
{

	if (inputManager->IsKeyPressed(DIK_W))
	{
		getCamera()->Forward((deltaTime*m_speed));

		// set camera node to the position of the camera
		setXPos(getCamera()->GetX());
		setYPos(getCamera()->GetY());
		setZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (m_sceneNode->check_collision(root_node, m_sceneNode) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Forward(-(deltaTime*m_speed));
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_S))
	{
		getCamera()->Forward(-(deltaTime*m_speed));

		// set camera node to the position of the camera
		setXPos(getCamera()->GetX());
		setYPos(getCamera()->GetY());
		setZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (m_sceneNode->check_collision(root_node, m_sceneNode) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Forward((deltaTime*m_speed));
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_A))
	{
		getCamera()->Sideways(-(deltaTime*m_speed));

		// set camera node to the position of the camera
		setXPos(getCamera()->GetX());
		setYPos(getCamera()->GetY());
		setZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (m_sceneNode->check_collision(root_node, m_sceneNode) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Sideways((deltaTime*m_speed));
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_D))
	{
		getCamera()->Sideways((deltaTime*m_speed));

		// set camera node to the position of the camera
		setXPos(getCamera()->GetX());
		setYPos(getCamera()->GetY());
		setZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (m_sceneNode->check_collision(root_node, m_sceneNode) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Sideways(-(deltaTime*m_speed));
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
}

void player::UpdateBullets(Scene_node* root_node, double deltaTime)
{
	for (int x = 0; x < bullets.size(); x++)
	{
		bullets[x]->UpdateBullet(root_node, deltaTime);
	}
}

void player::CheckFiring(InputManager* inputManager, double deltaTime)
{

	firingCooldown -= (deltaTime*0.05);
	if (firingCooldown < 0)
		firingCooldown = 0;


	bool loop = true;
	if (inputManager->IsKeyPressed(DIK_E) && firingCooldown == 0)
	{
		for (int x = 0; x < bullets.size(); x++)
		{
			if (!bullets[x]->IsActive() && loop)
			{
				bullets[x]->SetActive(m_xPos, m_yPos, m_zPos, m_playerCamera->GetRotationDX(), m_playerCamera->GetRotationDZ());
				if (m_shotgunPowerUp)
				{
					bullets[x + 1]->SetActive(m_xPos, m_yPos, m_zPos, m_playerCamera->GetRotationDX(), m_playerCamera->GetRotationDZ() + 0.2);
					bullets[x + 2]->SetActive(m_xPos, m_yPos, m_zPos, m_playerCamera->GetRotationDX(), m_playerCamera->GetRotationDZ() - 0.2);
				}
				firingCooldown = firingCooldownReset;
				loop = false;
			}
		}
	}
}

std::vector<bullet*> player::GetPlayerBullets()
{
	return bullets;
}

bool player::CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node)
{
	for (int i = 0; i < bullets.size(); i++)
	{

		if (bullets[i]->IsActive())
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
					bullets[i]->Deactivate();
					return true;
				}
			}
		}
	}

	return false;
}

void player::UpdatePlayer(InputManager* inputManager, Scene_node* actors_node, std::vector<bullet*> bullets, Scene_node* walls_node, double deltaTime)
{
	if (m_active)
	{
		m_damageTakenCooldown -= (deltaTime * 0.05);

		RotateCamera(inputManager);
		MoveCamera(inputManager, actors_node, deltaTime);
		CheckFiring(inputManager, deltaTime);
		UpdateBullets(walls_node, deltaTime);

		if (m_sceneNode->check_collision(actors_node, m_sceneNode))
		{
			if (CheckCollisionsBullets(bullets, actors_node))
			{

				if (m_damageTakenCooldown <= 0)
				{
					m_currHealth -= m_damageTaken;
					m_damageTaken = 0;

					m_damageTakenCooldown = m_damageTakenCooldownReset;
				}

				if (m_currHealth <= 0)
				{
					m_currHealth = 0;
					m_active = false;
				}
			}
		}
	}
}

void player::AddPlayerScore(int addScore)
{
	m_playerScore += addScore;
}

int player::GetPlayerScore()
{
	return m_playerScore;
}

int player::GetPlayerHealth()
{
	return m_currHealth;
}

void player::CheckHealthKitCollision(pickupHealth* healthKit)
{

	if (healthKit->IsActive())
	{
		//only check for collisions if both nodes contain a model
		if (this->getModel() && healthKit->getModel())
		{
			XMVECTOR v1 = this->getSceneNode()->GetWorldCentrePosition();
			XMVECTOR v2 = healthKit->getSceneNode()->GetWorldCentrePosition();
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
			if (sqrt(dx*dx + dy*dy + dz*dz) < (healthKit->getModel()->GetBoundingSphereRadius() * healthKit->getSceneNode()->GetWorldScale()) + (this->getModel()->GetBoundingSphereRadius() * this->getSceneNode()->GetWorldScale()))
			{
				healthKit->OnPickUp();
				m_currHealth += 10;
			}
		}
	}
}


void player::CheckPushableBlockCollision(pushableBlock* pushBlock, Scene_node* walls_node)
{
	//only check for collisions if both nodes contain a model
	if (this->getModel() && pushBlock->getModel())
	{
		XMVECTOR v1 = this->getSceneNode()->GetWorldCentrePosition();
		XMVECTOR v2 = pushBlock->getSceneNode()->GetWorldCentrePosition();
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
		if (sqrt(dx*dx + dy*dy + dz*dz) < (pushBlock->getModel()->GetBoundingSphereRadius() * pushBlock->getSceneNode()->GetWorldScale()) + (this->getModel()->GetBoundingSphereRadius() * this->getSceneNode()->GetWorldScale()))
		{
			pushBlock->MoveAwayFrom(m_xPos, m_zPos, walls_node);
		}
	}
}

void player::CheckShotgunCollision(pickupShotgun* shotgun)
{
	//only check for collisions if both nodes contain a model
	if (this->getModel() && shotgun->getModel())
	{
		XMVECTOR v1 = this->getSceneNode()->GetWorldCentrePosition();
		XMVECTOR v2 = shotgun->getSceneNode()->GetWorldCentrePosition();
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
		if (sqrt(dx*dx + dy*dy + dz*dz) < (shotgun->getModel()->GetBoundingSphereRadius() * shotgun->getSceneNode()->GetWorldScale()) + (this->getModel()->GetBoundingSphereRadius() * this->getSceneNode()->GetWorldScale()))
		{
			shotgun->OnPickUp();
			m_shotgunPowerUp = true;
		}
	}
}

bool player::IsActive()
{
	return m_active;
}
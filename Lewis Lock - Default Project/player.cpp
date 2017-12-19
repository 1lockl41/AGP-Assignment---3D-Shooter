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
		getCamera()->Pitch(inputManager->GetMouseState().lY * 0.001f);

		inputManager->SetLastMouseState(inputManager->GetMouseState());
	}
	//////////////////////////////////////////////////
}

void player::MoveCamera(InputManager* inputManager, Scene_node* root_node)
{

	if (inputManager->IsKeyPressed(DIK_W))
	{
		getCamera()->Forward(m_moveSpeed);

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
			getCamera()->Forward(-m_moveSpeed);
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());

		}
	}
	if (inputManager->IsKeyPressed(DIK_S))
	{
		getCamera()->Forward(-m_moveSpeed);

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
			getCamera()->Forward(m_moveSpeed);
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_A))
	{
		getCamera()->Sideways(-m_moveSpeed);

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
			getCamera()->Sideways(m_moveSpeed);
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_D))
	{
		getCamera()->Sideways(m_moveSpeed);

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
			getCamera()->Sideways(-m_moveSpeed);
			setXPos(getCamera()->GetX());
			setYPos(getCamera()->GetY());
			setZPos(getCamera()->GetZ());
		}
	}
}

void player::UpdateBullets()
{
	for (int x = 0; x < 10; x++)
	{
		bullets[x]->UpdateBullet();
	}
}

void player::CheckFiring(InputManager* inputManager)
{

	firingCooldown--;
	if (firingCooldown < 0)
		firingCooldown = 0;


	bool loop = true;
	if (inputManager->IsKeyPressed(DIK_E) && firingCooldown == 0)
	{
		for (int x = 0; x < 10; x++)
		{
			if (!bullets[x]->IsActive() && loop)
			{
				bullets[x]->SetActive(m_xPos, m_yPos, m_zPos, m_playerCamera->GetRotationDX(), m_playerCamera->GetRotationDZ());
				firingCooldown = firingCooldownReset;
				loop = false;
			}
		}
	}
}
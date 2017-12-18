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
	////////////////////////////////////////////
}

void player::MoveCamera(InputManager* inputManager, Scene_node* camera_node, Scene_node* root_node)
{

	if (inputManager->IsKeyPressed(DIK_W))
	{
		getCamera()->Forward(0.001f);

		// set camera node to the position of the camera
		camera_node->SetXPos(getCamera()->GetX());
		camera_node->SetYPos(getCamera()->GetY());
		camera_node->SetZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (camera_node->check_collision(root_node, camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Forward(-0.001f);
			camera_node->SetXPos(getCamera()->GetX());
			camera_node->SetYPos(getCamera()->GetY());
			camera_node->SetZPos(getCamera()->GetZ());

		}
	}
	if (inputManager->IsKeyPressed(DIK_S))
	{
		getCamera()->Forward(-0.001f);

		// set camera node to the position of the camera
		camera_node->SetXPos(getCamera()->GetX());
		camera_node->SetYPos(getCamera()->GetY());
		camera_node->SetZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (camera_node->check_collision(root_node, camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Forward(0.001f);
			camera_node->SetXPos(getCamera()->GetX());
			camera_node->SetYPos(getCamera()->GetY());
			camera_node->SetZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_A))
	{
		getCamera()->Sideways(-0.001f);

		// set camera node to the position of the camera
		camera_node->SetXPos(getCamera()->GetX());
		camera_node->SetYPos(getCamera()->GetY());
		camera_node->SetZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (camera_node->check_collision(root_node, camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Sideways(0.001f);
			camera_node->SetXPos(getCamera()->GetX());
			camera_node->SetYPos(getCamera()->GetY());
			camera_node->SetZPos(getCamera()->GetZ());
		}
	}
	if (inputManager->IsKeyPressed(DIK_D))
	{
		getCamera()->Sideways(0.001f);

		// set camera node to the position of the camera
		camera_node->SetXPos(getCamera()->GetX());
		camera_node->SetYPos(getCamera()->GetY());
		camera_node->SetZPos(getCamera()->GetZ());

		XMMATRIX identity = XMMatrixIdentity();

		// update tree to reflect new camera position
		root_node->UpdateCollisionTree(&identity, 1.0);

		if (camera_node->check_collision(root_node, camera_node) == true)
		{
			// if there is a collision, restore camera and camera node positions
			getCamera()->Sideways(-0.001f);
			camera_node->SetXPos(getCamera()->GetX());
			camera_node->SetYPos(getCamera()->GetY());
			camera_node->SetZPos(getCamera()->GetZ());
		}
	}
}
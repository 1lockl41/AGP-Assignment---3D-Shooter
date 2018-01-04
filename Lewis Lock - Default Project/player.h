#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"
#include "camera.h"
#include "InputManager.h"
#include "bullet.h"
#include "pickupHealth.h"
#include "pickupShotgun.h"
#include "pushableBlock.h"

//The player object. This is what the player will control and play the game through. 
class player : public baseClass
{
private:
	Camera* m_playerCamera = NULL; //Initialise the camera 
	
	//The cooldown between firing a bullet
	float firingCooldown;
	float firingCooldownReset;

	std::vector<bullet*> bullets; //Stores all bullets belonging to the player

	//Health of player
	int m_currHealth;
	int m_maxHealth;

	int m_damageTaken; //Damage taken from any colliding bullets

	//Cooldown between damage taken, prevents player from taking damage more than once from a single bullet. 
	float m_damageTakenCooldown;
	float m_damageTakenCooldownReset;

	//Current score, increases and player kills enemies
	int m_playerScore;

	//Whether or not the player has unlocked the shotgun power up
	bool m_shotgunPowerUp;

public:

	player(bool isSkybox, int xPos, int yPos, int zPos, Scene_node* actors_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(isSkybox, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_active = true; //begin active
		firingCooldownReset = 25;
		firingCooldown = firingCooldownReset;
		m_speed = 0.015f;
		m_playerScore = 0;

		m_playerCamera = new Camera(getXPos(), getYPos(), getZPos()-0.5f);

		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetBelongsToPlayer(true);
		m_sceneNode->SetIsPlayer(true);
		//m_sceneNode->SetScale(2);

		m_maxHealth = 50;
		m_currHealth = m_maxHealth;
		m_damageTaken = 0;
		m_damageTakenCooldownReset = 20;
		m_damageTakenCooldown = m_damageTakenCooldownReset;

		m_shotgunPowerUp = false; //Start game without shotgun power up, must be collected

		m_active = true;

		actors_node->addChildNode(m_sceneNode);

		//Initialise bullets and add to object pool
		for (int x = 0; x < 10; x++)
		{
			bullets.push_back(new bullet(false, -100,-100,-100,true, actors_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox,pDepthWriteSolid, pDepthWrtieSkybox));
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
			bullets[x]->getSceneNode()->SetBelongsToPlayer(true);
		}
	};

	Camera* getCamera();

	void RotateCamera(InputManager* inputManager); //Rotate camera based on mouse movement
	void MoveCamera(InputManager* inputManager, Scene_node* root_node, double deltaTime); //Move camera based on keyboard input

	void UpdateBullets(Scene_node* root_node, double deltaTime); //Update all bullets belonging to the player, moving them and checking collisions
	void CheckFiring(InputManager* inputManager, double deltaTime); //Check wether the player can fire again. Shoot a bullet when key pressed

	//Return all bullets belonging to the player
	std::vector<bullet*> GetPlayerBullets();

	//Check wether or not the player is colliding with any enemy bullets
	bool CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node);

	//Update player movement and collision
	void UpdatePlayer(InputManager* inputManager, Scene_node* actors_node, std::vector<bullet*> bullets, Scene_node* walls_node, double deltaTime);

	void AddPlayerScore(int addScore); //Add points to score
	int GetPlayerScore(); //Get score
	int GetPlayerHealth(); //Get current health

	void CheckHealthKitCollision(pickupHealth* healthKit); //Check whether player is colliding with a health kit
	void CheckShotgunCollision(pickupShotgun* shotgun); //check whether player is colliding with a shotgun pick up
	void CheckPushableBlockCollision(pushableBlock* pushBlock, Scene_node* walls_node); //Check wether the player is colliding with a pushable block.

	bool IsActive();
};
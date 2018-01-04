#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"
#include "player.h"
#include "ParticleGenerator.h"

//The enemy class, these AI will move towards the player once they are in range and will fire bullets at them.
//Can be killed by the player by shooting at them
class enemy : public baseClass
{
private:
	float m_firingCooldownReset; //The length of time between each shot the enemy fires
	float m_firingCooldown; //The current time between shots, resets to above value

	//Stores all bullet objects which belong to this enemy
	std::vector<bullet*> bullets;

	int m_currHealth; //current health
	int m_maxHealth; //max health, start at this value

	int m_damageTaken; //the damage taken from the last bullet collided with
	float m_damageTakenCooldown; //the current time since last damage taken
	float m_damageTakenCooldownReset; //the length of time between each damage the enemy can take, prevents them from being repeatedly hit by a single bullet
	
	float m_sightRange; //the range at which the enemy can detect the player

	XMVECTOR m_dir; //current direction of movement

	ParticleGenerator* particleGenerator = NULL; //The particle generator, creates and explosion on death

public:

	enemy(bool isSkybox, int xPos, int yPos, int zPos, Scene_node* actors_node, Scene_node* particles_node, char* bulletModelFileName, char* bulletTextureFileName, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWriteSkybox) : baseClass(isSkybox, xPos, yPos, zPos,modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWriteSkybox)
	{
		//Initialise firing cooldown timing
		m_firingCooldownReset = 50;
		m_firingCooldown = m_firingCooldownReset;

		m_speed = 0.0035f; //set speed
		m_active = false; //start inactive
		m_sightRange = 50; //set sight range

		//Set up health values
		m_maxHealth = 30;
		m_currHealth = m_maxHealth;

		//Initialise damage taken and damage cooldown
		m_damageTaken = 0;
		m_damageTakenCooldownReset = 20;
		m_damageTakenCooldown = m_damageTakenCooldownReset;

		//Set up scene node and model
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetBelongsToEnemy(true); 
		m_sceneNode->SetIsEnemy(true);
		actors_node->addChildNode(m_sceneNode);
		
		m_dir = XMVectorSet(0.0, 1.0, 0.0, 0.0); //Initialise current direction

		//Initialise 10 bullets which belong to this enemy.
		for (int x = 0; x < 10; x++)
		{
			bullets.push_back(new bullet(false, -100,-100,-100,false, actors_node, bulletModelFileName, bulletTextureFileName, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWriteSkybox));
			bullets[x]->setZPos(20);
			bullets[x]->setXPos(x);
			bullets[x]->getSceneNode()->SetBelongsToEnemy(true);
		}

		//Initialise particle generator
		particleGenerator = new ParticleGenerator(30,particles_node, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWriteSkybox);
	};


	void UpdateBullets(Scene_node* root_node, double deltaTime); //Updates all bullets belonging to this enemy, moves them and checks their collision
	bool CheckCollisionsBullets(std::vector<bullet*> bullets, Scene_node* root_node); //Check if this enemy has been hit by a player's bullet 
	void UpdateEnemy(std::vector<bullet*> bullets, Scene_node* root_node, float x_lookAt, float y_lookAt, player* player1, double deltaTime); //Update this enemy, moving them and checking collision
	void MoveTowards(Scene_node* root_node, float x_lookAt, float z_lookAt, double deltaTime); //The point to move towards, will set what direction they face and where they move to
	void CheckFiring(float x_lookAt, float y_lookAt, double deltaTime); //Check whether the enemy can fire a new bullet

	std::vector<bullet*> GetBullets(); //Get all bullets belonging to this enemy

	void SetActive(float xPos, float yPos, float zPos); //Spawn enemy, setting them as active and their position
	bool IsActive(); //check if active
};
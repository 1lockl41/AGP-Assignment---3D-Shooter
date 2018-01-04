#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"

//Bullet class, will be fired by both the player and enemies. Moves forward in a set direction until colliding with an object
class bullet : public baseClass
{
private:

	//Sets the bullet's lifetime once activated
	float m_activeTime;
	float m_activeTimeReset;

	//The damage will the bullet deals when colliding with an actor
	int m_damage;

	//Variables used for movement and direction
	XMVECTOR m_defaultForward;
	XMMATRIX m_bulletRotationMatrix;
	XMVECTOR m_defaultRight;
	XMVECTOR m_defaultUp;
	XMVECTOR m_bulletForward;
	XMVECTOR m_bulletRight;
	float m_moveLeftRight;
	float m_moveBackForward;
	XMVECTOR m_position, m_lookat, m_up;

	XMVECTOR m_dir;

public:

	bullet(bool isSkybox, int xPos, int yPos, int zPos, bool belongsToPlayer, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox) : baseClass(isSkybox, xPos, yPos, zPos, modelFilename, textureFilename, pD3DDevice, pImmediateContext, pRasterSolid, pRasterSkybox, pDepthWriteSolid, pDepthWrtieSkybox)
	{
		m_sceneNode->SetModel(m_model); //set model
		m_sceneNode->SetScale(0.1); //set scale
		root_node->addChildNode(m_sceneNode); //set scene node

		m_speed = 0.02; //speed of movement
		m_active = false; //start inactive

		//Set life time of bullet once active
		m_activeTimeReset = 300;
		m_activeTime = m_activeTimeReset;

		m_damage = 10; //damage dealt by this bullet

		//Initialise variables used for movement and direction
		m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_bulletForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_bulletRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

		m_dir = XMVectorSet(0.0, 1.0, 0.0, 0.0);

		m_moveLeftRight = 0.0f;
		m_moveBackForward = 0.0f;
		m_position = XMVectorSet(m_xPos, m_yPos, m_zPos, 0.0);

		m_sceneNode->SetBelongsToPlayer(belongsToPlayer); //set whether or not this bullet belong to the player. If so, it will ignore collisions with the player, if not, it will ignore collisions with enemies
	};

	void moveForward(double deltaTime); //Move the bullet foward, used by player to move based on camera direction
	void UpdateBullet(Scene_node* root_node, double deltaTime); //Update bullet, moving it and checking collision
	void UpdateBulletRotation(); //Set the direction the bullet should be moving when fired by player, based on angle of camera
	void SetActive(float xPos, float yPos, float zPos, float dx, float dz); //Spawn bullet, setting active, position, and angle
	void Deactivate(); //Deactive this bullet

	int GetDamage(); //Get the damage this bullet deals

	bool IsActive(); //Check if active

	void MoveTowards(double deltaTime); //Move bullet forward based on direction, used by enemies when fired as they have a set direction to use, unlike player's camera
	void SetDirection(float x_lookAt, float y_lookAt); //Set the direction to look at and move, used when fired by an enemy
};
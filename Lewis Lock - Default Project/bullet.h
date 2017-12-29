#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "baseClass.h"


class bullet : public baseClass
{
private:
	float m_activeTime;
	float m_activeTimeReset;

	int m_damage;

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
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(0.1);
		root_node->addChildNode(m_sceneNode);

		m_speed = 0.02;
		m_active = false;

		m_activeTimeReset = 300;
		m_activeTime = m_activeTimeReset;
		m_damage = 10;

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

		m_sceneNode->SetBelongsToPlayer(belongsToPlayer);
	};

	void moveForward(double deltaTime);
	void UpdateBullet(Scene_node* root_node, double deltaTime);
	void UpdateBulletRotation();
	void SetActive(float xPos, float yPos, float zPos, float dx, float dz);
	void Deactivate();

	int GetDamage();

	bool IsActive();

	void MoveTowards(double deltaTime);
	void SetDirection(float x_lookAt, float y_lookAt);
};
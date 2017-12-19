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
	float m_speed;
	bool m_active;
	float m_activeTime;
	float m_activeTimeReset;

	XMVECTOR m_defaultForward;
	XMMATRIX m_bulletRotationMatrix;
	XMVECTOR m_defaultRight;
	XMVECTOR m_defaultUp;
	XMVECTOR m_bulletForward;
	XMVECTOR m_bulletRight;
	float m_moveLeftRight;
	float m_moveBackForward;
	XMVECTOR m_position, m_lookat, m_up;
	float m_dx, m_dz;

public:

	bullet(bool belongsToPlayer, Scene_node* root_node, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext) : baseClass(modelFilename, textureFilename, pD3DDevice, pImmediateContext)
	{
		m_sceneNode->SetModel(m_model);
		m_sceneNode->SetScale(1);
		root_node->addChildNode(m_sceneNode);
		m_speed = 0.0001;
		m_active = false;
		m_activeTimeReset = 1000;
		m_activeTime = m_activeTimeReset;

		m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		m_bulletForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		m_bulletRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

		m_moveLeftRight = 0.0f;
		m_moveBackForward = 0.0f;
		m_position = XMVectorSet(m_xPos, m_yPos, m_zPos, 0.0);

		m_dx = 0.0f;
		m_dz = 0.0f;

		m_sceneNode->SetBelongsToPlayer(belongsToPlayer);
	};

	void moveForward();
	void UpdateBullet();
	XMMATRIX UpdateBulletRotation();
	void SetActive(float xPos, float yPos, float zPos, float dx, float dz);

	bool IsActive();
};
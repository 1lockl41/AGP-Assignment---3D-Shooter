#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

class Camera
{
public:
	Camera(float x, float y, float z, float camera_rotation);
	void Forward(float distance);
	void Sideways(float distance);
	XMMATRIX GetViewMatrix();
	float GetX();
	float GetY();
	float GetZ();
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);

	void MoveX(float xSpeed);
	void MoveY(float ySpeed);
	void MoveZ(float zSpeed);
	void Yaw(float degrees);
	void Pitch(float degrees);

	float GetRotationDX();
	float GetRotationDZ();

private:
	float m_x, m_y, m_z, m_dx, m_dz, m_camera_rotation;
	XMVECTOR m_position, m_lookat, m_up;

	XMVECTOR m_defaultForward;
	XMMATRIX m_cameraRotationMatrix;
	XMVECTOR m_defaultRight;
	XMVECTOR m_cameraForward;
	XMVECTOR m_cameraRight;
	float m_moveLeftRight;
	float m_moveBackForward;
};
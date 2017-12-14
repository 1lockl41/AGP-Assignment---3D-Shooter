#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "camera.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

Camera::Camera(float x, float y, float z, float camera_rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = camera_rotation;

	/*m_dx = sin(camera_rotation);
	m_dz = cos(camera_rotation);*/

	m_dx = 0.0f;
	m_dz = 0.0f;

	m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_cameraForward  = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_cameraRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
}

void Camera::Rotate(float degrees)
{
	/*m_camera_rotation += degrees;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));*/

}

void Camera::Forward(float distance)
{
	m_moveBackForward += distance;
	GetViewMatrix();
}

void Camera::Sideways(float distance)
{
	m_moveLeftRight += distance;
	GetViewMatrix();
}

void Camera::Yaw(float degrees)
{
	m_dz += degrees;
	GetViewMatrix();
}

void Camera::Pitch(float degrees)
{
	m_dx += degrees;
	GetViewMatrix();
}

XMMATRIX Camera::GetViewMatrix()
{
	//m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	//m_lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0);
	//m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	m_cameraRotationMatrix = XMMatrixRotationRollPitchYaw(m_dx, m_dz, 0);
	m_lookat = XMVector3TransformCoord(m_defaultForward, m_cameraRotationMatrix);
	m_lookat = XMVector3Normalize(m_lookat);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(m_dz);

	m_cameraRight = XMVector3TransformCoord(m_defaultRight, RotateYTempMatrix);
	m_up = XMVector3TransformCoord(m_up, RotateYTempMatrix);
	m_cameraForward = XMVector3TransformCoord(m_defaultForward, RotateYTempMatrix);

	m_position += m_moveLeftRight*m_cameraRight;
	m_position += m_moveBackForward*m_cameraForward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	m_lookat = m_position + m_lookat;

	m_x = XMVectorGetX(m_position);
	m_y = XMVectorGetY(m_position);
	m_z = XMVectorGetZ(m_position);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

float Camera::GetX()
{
	return m_x;
}

float Camera::GetY()
{
	return m_y;
}

float Camera::GetZ()
{
	return m_z;
}

void Camera::SetX(float x)
{
	m_x = x;
}

void Camera::SetY(float y)
{
	m_y = y;
}

void Camera::SetZ(float z)
{
	m_z = z;
}

void Camera::MoveX(float xSpeed)
{
	m_x += xSpeed;
}

void Camera::MoveY(float ySpeed)
{
	m_y += ySpeed;
}

void Camera::MoveZ(float zSpeed)
{
	m_z += zSpeed;
}
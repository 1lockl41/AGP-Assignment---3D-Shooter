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

	m_dx = sin(camera_rotation);
	m_dz = cos(camera_rotation);
}

void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;

	m_dx = sin(m_camera_rotation * (XM_PI / 180));
	m_dz = cos(m_camera_rotation * (XM_PI / 180));
}

void Camera::Forward(float distance)
{
	m_x += distance * m_dx;
	m_z += distance * m_dz;
}

void Camera::Sideways(float distance)
{
	/*XMVECTOR dir = XMVectorSet(m_x, 0.0, m_z,0.0);
	XMVECTOR v1 = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	XMVECTOR v = XMVector3Cross(dir, v1);
	m_x += XMVectorGetX(v) * distance;
	m_z += XMVectorGetZ(v) * distance;*/
	
	GetViewMatrix();

	XMVECTOR forward = XMVector3Normalize(m_lookat - m_position);
	XMVECTOR right = XMVector3Cross(forward, m_up);

	m_position -= (right * distance);
	m_x = XMVectorGetX(m_position);
	m_z = XMVectorGetZ(m_position);
}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

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
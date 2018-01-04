#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

//The camera object, moves based on player's position and direction. Used to draw the game
class Camera
{
public:
	Camera(float x, float y, float z);
	void Forward(float distance); //Move camera back and forward
	void Sideways(float distance); //Move camera left and right
	XMMATRIX GetViewMatrix(); //Get the current view
	float GetX();
	float GetY();
	float GetZ();
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);

	//Move in specifc directions, and rotation on yaw and pitch, looking up, down, left and right. Pitch is currently not used
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
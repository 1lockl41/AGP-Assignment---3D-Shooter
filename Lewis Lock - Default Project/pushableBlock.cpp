#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "pushableBlock.h"


void pushableBlock::MoveAwayFrom(float x_lookAt, float z_lookAt, Scene_node* root_node)
{
	m_dir = XMVectorSet(x_lookAt - m_xPos, 0.0, z_lookAt - m_zPos, 0.0);
	m_dir = XMVector3Normalize(m_dir);

	m_sceneNode->IncSetX(m_xPos - (XMVectorGetX(m_dir)*m_speed), root_node);
	m_sceneNode->IncSetZ(m_zPos - (XMVectorGetZ(m_dir)*m_speed), root_node);

	setXPos(m_sceneNode->GetXPos());
	setZPos(m_sceneNode->GetZPos());

}

bool pushableBlock::CheckIfTriggered(float zPos)
{
	if (getZPos() > zPos)
	{
		return true;
	}
	else
	{
		return false;
	}

}
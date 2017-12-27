#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "wall.h"

void wall::SetActive(bool active)
{
	m_active = active;

}

void wall::Update()
{
	if (!m_active)
	{
		setXPos(-100);
		setYPos(-100);
		setZPos(-100);
	}


}
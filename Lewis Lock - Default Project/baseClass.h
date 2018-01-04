#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"
#include "scene_node.h"

//The base class, all objects will inherit from this object. Handles model, texturing, position and scene node management
class baseClass
{
protected:

	Model* m_model = NULL; //Initilise the model which will display
	float m_xPos, m_yPos, m_zPos; //The current position of this object
	float m_xAngle, m_yAngle, m_zAngle; //The current angle of this object
	ID3D11ShaderResourceView* m_pTexture = NULL; //Initialise the texture used by this object
	ID3D11SamplerState* m_pSampler = NULL; //Initialise the sampler used by this object
	Scene_node* m_sceneNode = NULL; //The scene node which this object will belong to
	bool m_active; //Whether or not this object is active
	float m_speed; //The speed at which this object moves

public:
	baseClass(bool isSkybox, int xPos, int yPos, int zPos, char* modelFilename, char* textureFilename, ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	
	Model* getModel(); //Returns the object's model
	Scene_node* getSceneNode(); //Returns the scene node this object belongs to

	//Set this objects position and scale
	void setXPos(float x); 
	void setYPos(float y);
	void setZPos(float z);
	void setScale(float scale);

	//Get this object's current position
	float getXPos();
	float getYPos();
	float getZPos();

	//Face this object towards a point
	void LookAt_XZ(float xPos, float zPos);
};
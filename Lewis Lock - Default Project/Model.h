#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "objfilemodel.h"

class Model
{
private:
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	ObjFileModel* m_pObject;
	ID3D11VertexShader* m_pVShader;
	ID3D11PixelShader* m_pPShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pConstantBuffer;

	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState* m_pSampler0;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	float m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, m_bounding_sphere_radius;

	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();
	XMVECTOR GetBoundingSphereWorldSpacePosition();



public:
	Model(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext);
	bool LoadObjModel(char* filename);
	void Draw(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, XMVECTOR directional_light_colour, XMVECTOR ambient_light_colour, XMVECTOR directional_light_shines_from);
	~Model();
	//void SetXPos(float x);
	//void SetYPos(float y);
	//void SetZPos(float z);
	//float GetXPos();
	//float GetYPos();
	//float GetZPos();
	//void LookAt_XZ(float x, float z);
	//void MoveForward();
	bool CheckCollisions(Model* model);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetSampler(ID3D11SamplerState* sampler);
	float GetBoundingSphereX();
	float GetBoundingSphereY();
	float GetBoundingSphereZ();
	float GetBoundingSphereRadius();
};
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "objfilemodel.h"

//Model class, attatched to any object which has a model. Draws model and sets up collision information for scene node
class Model
{
private:
	ID3D11Device* m_pD3DDevice = NULL;
	ID3D11DeviceContext* m_pImmediateContext = NULL;

	ID3D11RasterizerState* m_pRasterSolid = NULL;
	ID3D11RasterizerState* m_pRasterSkybox = NULL;
	ID3D11DepthStencilState* m_pDepthWriteSolid = NULL;
	ID3D11DepthStencilState* m_pDepthWriteSkybox = NULL;

	ObjFileModel* m_pObject = NULL;
	ID3D11VertexShader* m_pVShader = NULL;
	ID3D11PixelShader* m_pPShader = NULL;
	ID3D11InputLayout* m_pInputLayout = NULL;
	ID3D11Buffer* m_pConstantBuffer = NULL;

	ID3D11ShaderResourceView* m_pTexture0 = NULL;
	ID3D11SamplerState* m_pSampler0 = NULL;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	float m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, m_bounding_sphere_radius;

	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();
	XMVECTOR GetBoundingSphereWorldSpacePosition();

	bool m_isSkybox;



public:
	Model(ID3D11Device* pD3DDevice, ID3D11DeviceContext* pImmediateContext, ID3D11RasterizerState* pRasterSolid, ID3D11RasterizerState* pRasterSkybox, ID3D11DepthStencilState* pDepthWriteSolid, ID3D11DepthStencilState* pDepthWrtieSkybox);
	bool LoadObjModel(char* filename);
	void Draw(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, XMVECTOR directional_light_colour, XMVECTOR ambient_light_colour, XMVECTOR directional_light_shines_from);
	~Model();

	void SetYangle(float y);

	//void MoveForward();
	bool CheckCollisions(Model* model);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetSampler(ID3D11SamplerState* sampler);
	float GetBoundingSphereX();
	float GetBoundingSphereY();
	float GetBoundingSphereZ();
	float GetBoundingSphereRadius();
	void SetIsSkybox(bool isSkybox);

};
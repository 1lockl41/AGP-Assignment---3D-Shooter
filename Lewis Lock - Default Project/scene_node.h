#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"

class Scene_node
{
private:
	Model* m_p_model;
	vector<Scene_node*> m_children;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	float m_world_centre_x, m_world_centre_y, m_world_centre_z;
	float m_world_scale;

	bool m_belongsToPlayer;
	bool m_belongsToEnemy;

public:
	Scene_node();
	void SetModel(Model* model);
	void SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);
	float GetXPos();
	float GetYPos();
	float GetZPos();
	void SetScale(float scale);
	float GetScale();

	void addChildNode(Scene_node *n);
	bool detatchNode(Scene_node *n);
	void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, XMVECTOR directional_light_colour, XMVECTOR ambient_light_colour, XMVECTOR directional_light_shines_from);

	XMVECTOR GetWorldCentrePosition();

	void UpdateCollisionTree(XMMATRIX* world, float scale);
	bool check_collision(Scene_node* compare_tree, Scene_node* object_tree_root, bool belongsToPlayer);

	bool IncX(float in, Scene_node* root_node);
	bool IncY(float in, Scene_node* root_node);
	bool IncZ(float in, Scene_node* root_node);

	bool GetBelongsToPlayer();
	void SetBelongsToPlayer(bool belongsToPlayer);

	bool GetBelongsToEnemy();
	void SetBelongsToEnemy(bool belongsToEnemy);

	float GetWorldScale();

	//bool check_collision_bullets(std::vector<bullet*> bullets);
};
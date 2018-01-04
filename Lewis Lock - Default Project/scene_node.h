#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Model.h"

//Scene node management, handles drawing and collision of objects in different group.
//These groups of objects have been described in 'game.h'
class Scene_node
{
private:
	Model* m_p_model = NULL; //The model attatched to this node
	vector<Scene_node*> m_children; //All the children of this node

	float m_x, m_y, m_z; //current position
	float m_xangle, m_zangle, m_yangle; //current angle
	float m_oldyangle;
	float m_scale; //scale of model

	float m_world_centre_x, m_world_centre_y, m_world_centre_z; //centre of object in world space
	float m_world_scale; //scale in world space

	bool m_isPlayer; //if node is a player
	bool m_isEnemy; //if node is an enemy
	bool m_belongsToPlayer; //if node belongs to the player, allows player to ignore collision with their own bullets
	bool m_belongsToEnemy; //if node belongs to an enemy, allows enemies to ignore collisions with their own bullets, or a fellow enemy's bullets

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

	void addChildNode(Scene_node *n); //Add a child node
	bool detatchNode(Scene_node *n); //Remove node from its parent
	void execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, XMVECTOR directional_light_colour, XMVECTOR ambient_light_colour, XMVECTOR directional_light_shines_from); //Draw this node and any children

	XMVECTOR GetWorldCentrePosition();

	void UpdateCollisionTree(XMMATRIX* world, float scale); //Update the collision tree of nodes
	bool check_collision(Scene_node* compare_tree, Scene_node* object_tree_root); //check if there is a collision occuring between nodes

	//Increment the position of this node, checking for collisions. If a collision occurs, move to old position
	bool IncX(float in, Scene_node* root_node);
	bool IncY(float in, Scene_node* root_node);
	bool IncZ(float in, Scene_node* root_node);

	//Set the position of this node, checking for collisions. If a collision occurs, move to old position
	bool IncSetX(float xPos, Scene_node* root_node);
	bool IncSetY(float yPos, Scene_node* root_node);
	bool IncSetZ(float zPos, Scene_node* root_node);

	bool GetBelongsToPlayer();
	void SetBelongsToPlayer(bool belongsToPlayer);

	bool GetBelongsToEnemy();
	void SetBelongsToEnemy(bool belongsToEnemy);

	bool GetIsPlayer();
	void SetIsPlayer(bool isPlayer);

	bool GetIsEnemy();
	void SetIsEnemy(bool isEnemy);

	float GetWorldScale();

	void SetYangle(float y);
};
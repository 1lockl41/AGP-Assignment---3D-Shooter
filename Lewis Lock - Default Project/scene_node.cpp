#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "scene_node.h"


Scene_node::Scene_node()
{
	m_p_model = NULL;

	m_x, m_y, m_z = 0.0f;
	m_xangle, m_zangle, m_yangle = 0.0f;
	m_oldyangle = 0.0f;
	m_scale = 1.0f;

	m_belongsToPlayer = false;
	m_belongsToEnemy = false;
	m_isPlayer = false;
	m_isEnemy = false;
}

void Scene_node::SetModel(Model* model)
{
	m_p_model = model;
}

void Scene_node::SetXPos(float x)
{
	m_x = x;
}

void Scene_node::SetYPos(float y)
{
	m_y = y;
}

void Scene_node::SetZPos(float z)
{
	m_z = z;
}

float Scene_node::GetXPos()
{
	return m_x;
}

float Scene_node::GetYPos()
{
	return m_y;
}

float Scene_node::GetZPos()
{
	return m_z;
}

void Scene_node::SetScale(float scale)
{
	m_scale = scale;
}

float Scene_node::GetScale()
{
	return m_scale;
}

void Scene_node::addChildNode(Scene_node *n)
{
	m_children.push_back(n);
}

bool Scene_node::detatchNode(Scene_node *n)
{
	// traverse tree to find node to detatch
	for (int i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}
		if (m_children[i]->detatchNode(n) == true) return true;
	}
	return false; // node not in this tree
}

void Scene_node::execute(XMMATRIX *world, XMMATRIX* view, XMMATRIX* projection, XMVECTOR directional_light_colour, XMVECTOR ambient_light_colour, XMVECTOR directional_light_shines_from)
{
	// the local_world matrix will be used to calc the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	if (m_yangle != 0 && m_yangle != m_oldyangle)
	{
		m_oldyangle = m_yangle;
	}
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	local_world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// only draw if there is a model attached
	if (m_p_model) m_p_model->Draw(&local_world, view, projection, directional_light_colour, ambient_light_colour, directional_light_shines_from);

	// traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i< m_children.size(); i++)
	{
		m_children[i]->execute(&local_world, view, projection, directional_light_colour, ambient_light_colour, directional_light_shines_from);
	}
}

XMVECTOR Scene_node::GetWorldCentrePosition()
{
	return XMVectorSet(m_world_centre_x, m_world_centre_y, m_world_centre_z, 0.0);
}

void Scene_node::UpdateCollisionTree(XMMATRIX* world, float scale)
{
	// the local_world matrix will be used to calculate the local transformations for this node
	XMMATRIX local_world = XMMatrixIdentity();

	local_world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	local_world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	local_world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	local_world *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	//calculate the world space scale of this object, is needed to calculate the
	//correct bounding sphere radius of an object in a scaled hierarchy
	m_world_scale = scale * m_scale;

	XMVECTOR v;
	if (m_p_model)
	{
		v = XMVectorSet(m_p_model->GetBoundingSphereX(), m_p_model->GetBoundingSphereY(), m_p_model->GetBoundingSphereZ(), 0.0);
	}
	else
	{
		v = XMVectorSet(0, 0, 0, 0); //no model, default to 0
	}

	//find and store world spcae bounding sphere centre
	v = XMVector3Transform(v, local_world);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	//traverse all child nodes, passing in the concatenated world matrix and scale
	for (int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->UpdateCollisionTree(&local_world, m_world_scale);
	}
}

bool Scene_node::check_collision(Scene_node* compare_tree, Scene_node* object_tree_root)
{
	// check to see if root of tree being compared is same as root node of object tree being checked
	// i.e. stop object node and children being checked against each other
	if (object_tree_root == compare_tree) return false;

	if (object_tree_root->GetBelongsToPlayer() && compare_tree->GetBelongsToPlayer())
	{
		return false;
	}

	if (object_tree_root->GetBelongsToEnemy() && compare_tree->GetBelongsToEnemy())
	{
		return false;
	}

	//if (object_tree_root->GetBelongsToPlayer() && compare_tree->GetBelongsToEnemy())
	//{
	//	return false;
	//}

	//if (object_tree_root->GetBelongsToEnemy() && compare_tree->GetBelongsToPlayer())
	//{
	//	return false;
	//}

	// only check for collisions if both nodes contain a model
	if (m_p_model && compare_tree->m_p_model)
	{
		XMVECTOR v1 = GetWorldCentrePosition();
		XMVECTOR v2 = compare_tree->GetWorldCentrePosition();
		XMVECTOR vdiff = v1 - v2;

		//XMVECTOR a = XMVector3Length(vdiff);
		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		// check bounding sphere collision
		if (sqrt(dx*dx + dy*dy + dz*dz) < (compare_tree->m_p_model->GetBoundingSphereRadius() * compare_tree->m_world_scale) + (this->m_p_model->GetBoundingSphereRadius() * m_world_scale))
		{
			return true;
		}
	}

	// iterate through compared tree child nodes
	for (int i = 0; i< compare_tree->m_children.size(); i++)
	{
		// check for collsion against all compared tree child nodes 
		if (check_collision(compare_tree->m_children[i], object_tree_root) == true) return true;
	}

	// iterate through composite object child nodes
	for (int i = 0; i< m_children.size(); i++)
	{
		// check all the child nodes of the composite object against compared tree
		if (m_children[i]->check_collision(compare_tree, object_tree_root) == true) return true;
	}

	return false;
}

bool Scene_node::IncX(float in, Scene_node* root_node)
{
	float old_x = m_x;	// save current state 
	m_x += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_x = old_x;

		return true;
	}

	return false;
}

bool Scene_node::IncY(float in, Scene_node* root_node)
{
	float old_y = m_y;	// save current state 
	m_y += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	//root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_y = old_y;

		return true;
	}

	return false;
}

bool Scene_node::IncZ(float in, Scene_node* root_node)
{
	float old_z = m_z;	// save current state 
	m_z += in;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	//root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_z = old_z;

		return true;
	}

	return false;
}


bool Scene_node::IncSetX(float xPos, Scene_node* root_node)
{
	float old_x = m_x;	// save current state 
	m_x = xPos;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	//root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_x = old_x;

		return true;
	}

	return false;
}

bool Scene_node::IncSetY(float yPos, Scene_node* root_node)
{
	float old_y = m_y;	// save current state 
	m_y = yPos;	// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_y = old_y;

		return true;
	}

	return false;
}

bool Scene_node::IncSetZ(float zPos, Scene_node* root_node)
{
	float old_z = m_z;	// save current state 
	m_z = zPos;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	root_node->UpdateCollisionTree(&identity, 1.0);

	// check for collision of this node (and children) against all other nodes
	if (check_collision(root_node, this) == true)
	{
		// if collision restore state
		m_z = old_z;

		return true;
	}

	return false;
}

bool Scene_node::GetBelongsToPlayer()
{
	return m_belongsToPlayer;
}

void Scene_node::SetBelongsToPlayer(bool belongsToPlayer)
{
	m_belongsToPlayer = belongsToPlayer;
}

bool Scene_node::GetBelongsToEnemy()
{
	return m_belongsToEnemy;

}

void Scene_node::SetBelongsToEnemy(bool belongsToEnemy)
{
	m_belongsToEnemy = belongsToEnemy;
}


bool Scene_node::GetIsPlayer()
{
	return m_isPlayer;
}

void Scene_node::SetIsPlayer(bool isPlayer)
{
	m_isEnemy = isPlayer;
}

bool Scene_node::GetIsEnemy()
{
	return m_isEnemy;
}

void Scene_node::SetIsEnemy(bool isEnemy)
{
	m_isEnemy = isEnemy;
}

float Scene_node::GetWorldScale()
{
	return m_world_scale;
}

void Scene_node::SetYangle(float y)
{
	m_yangle = y;
}
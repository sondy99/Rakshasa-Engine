#include "Quadtree.h"
#include "Globals.h"

#include "ModuleModelLoader.h"

#include "GameObject.h"
#include "ComponentMesh.h"

#define MAX_ITEMS 5
#define MIN_SIZE 1000.0f

Quadtree::Quadtree()
{
}

Quadtree::~Quadtree()
{
	Clear(true);
}

void Quadtree::InitQuadTree(const math::AABB& aabb, bool clearAllGameObjects)
{
	Clear(clearAllGameObjects);
	root = new QuadtreeNode(aabb);
}

void Quadtree::InsertGameObject(GameObject* gameObject, bool addToAllGameObjects)
{
	ComponentMesh* componentMesh = (ComponentMesh*)gameObject->GetComponent(ComponentType::MESH);
	if (root != nullptr && componentMesh != nullptr && componentMesh->globalBoundingBox.Intersects(root->aabb))
	{
		if (addToAllGameObjects)
		{
			allGameObjects.push_back(gameObject);
		}

		root->InsertGameObject(gameObject);
	}
}

void Quadtree::RemoveGameObject(GameObject * gameObject)
{
	if (root != nullptr)
	{
		allGameObjects.remove(gameObject);

		InitQuadTree(math::AABB(math::float3(-40000.0f, 0.0f, -40000.0f), math::float3(40000.0f, 5000.0f, 40000.0f)), false);

		for (std::list<GameObject*>::iterator iterator = allGameObjects.begin(); iterator != allGameObjects.end(); ++iterator)
		{
			InsertGameObject(*iterator, false);
		}
	}
}

void Quadtree::Clear(bool clearAllGameObjects)
{
	if (clearAllGameObjects)
	{
		allGameObjects.clear();
	}

	RELEASE(root);
}

QuadtreeNode::QuadtreeNode()
{
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}

QuadtreeNode::QuadtreeNode(const math::AABB& aabb) : aabb(aabb)
{
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}

QuadtreeNode::~QuadtreeNode()
{
	RELEASE(childs[0]);
	RELEASE(childs[1]);
	RELEASE(childs[2]);
	RELEASE(childs[3]);
}

void QuadtreeNode::InsertGameObject(GameObject* gameObject)
{
	if (IsLeaf() && (gameObjectList.size() < MAX_ITEMS || aabb.HalfSize().LengthSq() <= MIN_SIZE * MIN_SIZE))
	{
		gameObjectList.push_back(gameObject);
	} 
	else
	{
		if (IsLeaf())
		{
			CreateChilds();
		}

		gameObjectList.push_back(gameObject);

		RedistributeGameObjects();
	}
}

void QuadtreeNode::CreateChilds()
{
	math::AABB newAABB;
	math::float3 aabbSize(aabb.Size());
	math::float3 newSize(aabbSize.x * 0.5f, aabbSize.y, aabbSize.z * 0.5f);
	math::float3 aabbCenter(aabb.CenterPoint());
	math::float3 newCenter(aabbCenter);

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[0] = new QuadtreeNode(newAABB);
	childs[0]->parent = this;

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[1] = new QuadtreeNode(newAABB);
	childs[1]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[2] = new QuadtreeNode(newAABB);
	childs[2]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[3] = new QuadtreeNode(newAABB);
	childs[3]->parent = this;
}

void QuadtreeNode::RedistributeGameObjects()
{
	for (std::list<GameObject*>::iterator iterator = gameObjectList.begin(); iterator != gameObjectList.end();)
	{
		GameObject* gameObject = *iterator;
		bool intersects[4];
		for (int i = 0; i < 4; ++i)
		{
			ComponentMesh* componentMesh = (ComponentMesh*)(gameObject)->GetComponent(ComponentType::MESH);
			if (componentMesh != nullptr)
			{
				intersects[i] = childs[i]->aabb.Intersects(componentMesh->globalBoundingBox);
			}
		}

		if (intersects[0] && intersects[1] && intersects[2] && intersects[3])
		{
			++iterator;
		}
		else
		{
			iterator = gameObjectList.erase(iterator);
			for (int i = 0; i < 4; ++i)
			{
				if (intersects[i])
				{
					childs[i]->InsertGameObject(gameObject);
				}
			}
		}
	}
}

bool QuadtreeNode::IsLeaf() const
{
	return childs[0] == nullptr;
}
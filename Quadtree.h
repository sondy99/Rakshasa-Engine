#ifndef __Quadtree_h__
#define __Quadtree_h__

#include "MathGeoLib.h"
#include <list>

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode();
	QuadtreeNode(const math::AABB& aabb);
	~QuadtreeNode();

	void InsertGameObject(GameObject* gameObject);
	void CreateChilds();
	void RedistributeGameObjects();
	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*>& gameObjectList, const TYPE& primitive) const;
	bool IsLeaf() const;

	math::AABB aabb;
	std::list<GameObject*> gameObjectList;
	QuadtreeNode* parent = nullptr;
	QuadtreeNode* childs[4];
};

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void InitQuadTree(const math::AABB& aabb, bool clearAllGameObjects);
	void InsertGameObject(GameObject* gameObject, bool addToAllGameObjects);
	void RemoveGameObject(GameObject* gameObject);
	void Clear(bool clearAllGameObjects);
	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*>& gameObjectList, const TYPE& primitive) const;

	QuadtreeNode* root = nullptr;
	std::list<GameObject*> allGameObjects;
};

template<typename TYPE>
inline void Quadtree::CollectIntersections(std::vector<GameObject*>& gameObject, const TYPE & primitive) const
{
	if (root != nullptr)
	{
		root->CollectIntersections(gameObject, primitive);
	}
}

template<typename TYPE>
inline void QuadtreeNode::CollectIntersections(std::vector<GameObject*>& gameObject, const TYPE & primitive) const
{
	if (primitive.Intersects(aabb))
	{
		for (std::list<GameObject*>::const_iterator it = this->gameObjectList.begin(); it != this->gameObjectList.end(); ++it)
		{
			ComponentMesh* componentMesh = (ComponentMesh*)(*it)->GetComponent(ComponentType::MESH);

			if (componentMesh != nullptr && primitive.Intersects(componentMesh->globalBoundingBox))
			{
				gameObject.push_back(*it);
			}

		}
		for (int i = 0; i < 4; ++i)
		{
			if (childs[i] != nullptr) childs[i]->CollectIntersections(gameObject, primitive);
		}
	}
}


#endif

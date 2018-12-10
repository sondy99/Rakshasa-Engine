#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Component.h"

#include "MathGeoLib.h"

#include <list>
#include <string>

class Component;

class GameObject
{
public:
	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void Update();

	void DrawProperties();

	void RemoveGameObject(GameObject* mainObjectToDelete);
	void DuplicateGameObject(GameObject* newGameObjectParent);
	GameObject* Clone();
	Component* GameObject::GetComponent(ComponentType componentType);

	void UpdateBoundingBox();
	void UpdateBoundingBoxTransformation();
public:
	char uuid[37];
	std::string name = nullptr;
	GameObject* parent = nullptr;
	bool isSelected = false;
	bool active = true;

	std::list<Component*> components;
	std::list<GameObject*> childrens;

	AABB originalboundingBox = AABB();
	AABB boundingBox = AABB();
};

#endif __GAMEOBJECT_H__


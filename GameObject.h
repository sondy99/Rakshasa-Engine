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

	void update();
	void remove(GameObject* mainObjectToDelete);
	void duplicate(GameObject* newGameObjectParent);
	GameObject* clone();
	Component* GameObject::GetComponent(ComponentType componentType);

	AABB GetBoundingBox() const;
public:
	char uuid[37];
	std::string name = nullptr;
	GameObject* parent = nullptr;
	bool isSelected = false;
	bool active = true;

	std::list<Component*> components;
	std::list<GameObject*> childrens;
private:
	AABB localBoundingBox;
};

#endif __GAMEOBJECT_H__


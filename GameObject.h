#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Component.h"

#include <vector>
#include <string>

class Component;

class GameObject
{
public:
	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void update();
	Component* GameObject::GetComponent(ComponentType componentType);
public:
	std::string uuid = "";
	std::string name = nullptr;
	GameObject* parent = nullptr;
	bool isSelected = false;
	bool active = true;

	std::vector<Component*> components;
	std::vector<GameObject*> childrens;
};

#endif __GAMEOBJECT_H__


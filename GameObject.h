#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"

#include <vector>

class Component;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void update();
public:
	const char* name = nullptr;
	GameObject* parent = nullptr;

	std::vector<Component*> components;
	std::vector<GameObject*> childrens;
};

#endif __GAMEOBJECT_H__


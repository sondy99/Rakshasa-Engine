#include "GameObject.h"
#include "crossguid/guid.hpp"

#include "Application.h"
#include "ModuleScene.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	xg::Guid guid = xg::newGuid();

	std::string uuidAux = guid.str();
	sprintf_s(uuid, uuidAux.c_str());
}

GameObject::~GameObject()
{
}

void GameObject::update()
{
}

void GameObject::remove()
{
	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end();)
	{
		RELEASE(*iterator);
		iterator = components.erase(iterator);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end();)
	{
		(*iterator)->remove();
		RELEASE(*iterator);
		iterator = childrens.erase(iterator);
	}

	if (parent == App->scene->root) 
	{
		parent->childrens.remove(this);
	}
}

Component* GameObject::GetComponent(ComponentType componentType)
{
	Component* result = nullptr;

	for (auto &component : components)
	{
		if (component->componentType == componentType)
		{
			result = component;
			break;
		}
	}

	return result;
}
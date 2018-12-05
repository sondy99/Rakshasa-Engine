#include "GameObject.h"
#include "crossguid/guid.hpp"

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
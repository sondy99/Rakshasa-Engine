#include "GameObject.h"
#include "crossguid/guid.hpp"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	xg::Guid guid = xg::newGuid();
	uuid = guid.str();
}

GameObject::~GameObject()
{
}

void GameObject::update()
{
}

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class GameObject;

enum class ComponentType
{
	TRANSFORM = 0,
	CAMERA,
	MESH,
	MATERIAL
};

class Component
{
public:
	Component(const GameObject* gameObjectParent, ComponentType componentType) : gameObjectParent(gameObjectParent), componentType(componentType) {};
	virtual ~Component() {};

	virtual void enable() {};
	virtual void update() {};
	virtual void disable() {};
public:
	ComponentType componentType;
	bool active = false;
	const GameObject* gameObjectParent = nullptr;
};

#endif __COMPONENT_H__
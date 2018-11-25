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
	Component(const GameObject* gameObject, ComponentType componentType) : gameObject(gameObject), componentType(componentType) {};
	virtual ~Component() {};

	virtual void enable() {};
	virtual void update() {};
	virtual void disable() {};
public:
	ComponentType componentType;
	bool active = false;
	const GameObject* gameObject = nullptr;
};

#endif __COMPONENT_H__
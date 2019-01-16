#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

#include "document.h"
#include "prettywriter.h"

class GameObject;
class Config;

enum class ComponentType
{
	TRANSFORMATION = 0,
	MESH,
	MATERIAL,
	CAMERA
};

class Component
{
public:
	Component() {};
	Component(GameObject* gameObjectParent, ComponentType componentType) : gameObjectParent(gameObjectParent), componentType(componentType) {};
	virtual ~Component() {};

	virtual void Enable() {};
	virtual void Update() {};
	virtual void Disable() {};
	virtual void DrawProperties() {};
	virtual Component* Clone() { return new Component(gameObjectParent, componentType); };

	virtual void Save(const Config* config) {};
	virtual void Load(const Config* config, rapidjson::Value& value) {};
public:
	ComponentType componentType;
	GameObject* gameObjectParent = nullptr;
	bool isMarkToBeDeleted = false;
};

#endif __COMPONENT_H__
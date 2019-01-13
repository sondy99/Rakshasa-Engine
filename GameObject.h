#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "Component.h"

#include "MathGeoLib.h"

#include <list>
#include <string>

#include "document.h"
#include "prettywriter.h"

class Component;
class Config;

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
	void GetComponentRecursive(std::list<Component*>& componentList, ComponentType componentType);
	bool GameObject::HaveComponentRecursive(ComponentType componentType);
	Component* CreateComponent(ComponentType componentType);

	bool Save(Config* config);
	void Load(Config* config, rapidjson::Value& value);
public:
	char uuid[37];
	std::string name = nullptr;
	GameObject* parent = nullptr;
	bool isSelected = false;
	bool active = true;
	bool gameObjectStatic = false;

	std::list<Component*> components;
	std::list<GameObject*> childrens;
private:
	std::list<Component*>::iterator RemoveComponent(std::list<Component*>::iterator iteratorToBeDeleted, Component* componentToBeRemove);
};

#endif __GAMEOBJECT_H__


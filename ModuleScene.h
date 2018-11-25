#ifndef __MODULESCENE_H_
#define __MODULESCENE_H_

#include "Module.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	void DrawProperties();
public:
	GameObject* root = nullptr;
	bool toggleSceneProperties = true;
private:
	void DrawTreeNode(GameObject * gameObject);
};

#endif __MODULESCENE_H_
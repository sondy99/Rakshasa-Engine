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

	void LoadModel(const char* modelPath);
	GameObject* CreateGameObject(const char* name, GameObject* parent);
	void DrawProperties();
public:
	GameObject* root = nullptr;
	bool toggleSceneProperties = true;
private:
	void DrawTreeNode(GameObject * gameObjectParent);
};

#endif __MODULESCENE_H_
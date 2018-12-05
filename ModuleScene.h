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
	GameObject* gameObjectSelected = nullptr;
	bool toggleSceneProperties = true;
private:
	void SetGameObjectSelected(GameObject* gameObject);
	void DrawTreeNode(GameObject * gameObjectParent);
	GameObject* ModuleScene::GetGameObjectByName(GameObject* gameObject, char uuidObjectName[37]);
};

#endif __MODULESCENE_H_
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
	GameObject* CreateGameObject(const char* name, GameObject* parent, bool withTransformation);
	void DrawProperties();
	GameObject* GetGameCamera();
public:
	GameObject* root = nullptr;
	GameObject* gameObjectSelected = nullptr;
	bool toggleSceneProperties = true;
private:
	void DrawTreeNode(GameObject * gameObject);
	void SetGameObjectSelected(GameObject* gameObject);
	GameObject* ModuleScene::GetGameObjectByUUID(GameObject* gameObject, char uuidObjectName[37]);
	void DragAndDropManagement(GameObject* gameObjectParent);
	void ClickManagement(GameObject* gameObject);
	void ManageDuplicationAndDeletionGameObject();
private:
	GameObject* gameObjectToBeDeleted = nullptr;
	GameObject* gameObjectToBeDuplicated = nullptr;
};

#endif __MODULESCENE_H_
#ifndef __MODULESCENE_H_
#define __MODULESCENE_H_

#include "Module.h"

#include "list";

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
	std::list<GameObject*> GetGameCameras();
public:
	GameObject* root = nullptr;
	GameObject* gameObjectSelected = nullptr;
	bool toggleSceneProperties = true;
private:
	void DrawTreeNode(GameObject * gameObject);
	void SetGameObjectSelected(GameObject* gameObject);
	GameObject* GetGameObjectByUUID(GameObject* gameObject, char uuidObjectName[37]);
	void DragAndDropManagement(GameObject* gameObjectParent);
	void ClickManagement(GameObject* gameObject);
	void ManageDuplicationAndDeletionGameObject();
private:
	GameObject* gameObjectToBeDeleted = nullptr;
	GameObject* gameObjectToBeDuplicated = nullptr;
	int gameObjectCounter = 1;
};

#endif __MODULESCENE_H_
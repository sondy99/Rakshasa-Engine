#ifndef __MODULESCENE_H_
#define __MODULESCENE_H_

#include "Module.h"

#include "MathGeoLib.h"

#include "list";

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;

	void LoadModel(const char* modelPath);
	void LoadTexture(const char* texturePath);
	GameObject* CreateGameObject(const char* name, GameObject* parent, bool withTransformation);
	void DrawProperties();
public:
	GameObject* root = nullptr;
	GameObject* gameObjectSelected = nullptr;
	bool toggleSceneProperties = true;
	bool isSceneCullingActive = true;
	math::float3 lightPosition = math::float3(5000.0f, 5000.0f, 8000.0f);
	float ambient = 0.1f;
private:
	void DrawGeometryGameObjectButtons(GameObject* gameObjectParent);
	void DrawTreeNode(GameObject * gameObject);
	void SetGameObjectSelected(GameObject* gameObject);
	GameObject* GetGameObjectByUUID(GameObject* gameObject, char uuidObjectName[37]);
	void DragAndDropManagement(GameObject* gameObjectParent);
	void ClickManagement(GameObject* gameObject);
	void ManageDuplicationAndDeletionGameObject();
	bool CheckIfOneGameObjectIsParentOfOther(const GameObject& gameObjectPossibleParent, const GameObject& gameObjectPossibleSon);
	void MoveUpDownGameObject(GameObject* gameObject, bool up);
private:
	GameObject* gameObjectToBeDeleted = nullptr;
	GameObject* gameObjectToBeDuplicated = nullptr;
	int gameObjectCounter = 1;
};

#endif __MODULESCENE_H_
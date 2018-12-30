#ifndef __MODULESCENE_H_
#define __MODULESCENE_H_

#include "Module.h"

#include "MathGeoLib.h"

#include "list";

#include "document.h"
#include "prettywriter.h"

class GameObject;
class Config;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent, bool withTransformation);
	void CreateGameObject(Config* config, rapidjson::Value & value);
	void DrawProperties();
public:
	GameObject* root = nullptr;
	GameObject* gameObjectSelected = nullptr;
	bool toggleSceneProperties = true;
	bool isSceneCullingActive = true;
	float ambientLight = 0.1f;
	math::float3 ambientLightPosition = math::float3(5000.0f, 5000.0f, 8000.0f);
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

	void SaveScene();
	void SaveGameObject(Config* config, GameObject* gameObject);
	void LoadScene();
private:
	GameObject* gameObjectToBeDeleted = nullptr;
	GameObject* gameObjectToBeDuplicated = nullptr;
	int gameObjectCounter = 1;
};

#endif __MODULESCENE_H_
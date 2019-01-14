#ifndef __MODULESCENE_H_
#define __MODULESCENE_H_

#include "Module.h"

#include "MathGeoLib.h"

#include "QuadTree.h"

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
	bool CleanUp() override;
	update_status Update() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent, bool withTransformation);
	void CreateGameObject(Config* config, rapidjson::Value & value);
	void DrawProperties();
	void ClearScene();
	void SetGameObjectSelected(GameObject* gameObject);
	inline GameObject* GetGameObjectSelected() const { return gameObjectSelected; }
public:
	GameObject* root = nullptr;
	bool toggleSceneProperties = true;
	bool isSceneCullingActive = true;
	float ambientLight = 0.1f;
	math::float3 ambientLightPosition = math::float3(5000.0f, 5000.0f, 8000.0f);
	static char sceneFileName[];
	static const char* labelCurrentSceneFileName;
	bool markToUpdateSceneFiles = false;

	Quadtree* quadTree = nullptr;
	bool drawQuadTree = false;
	bool drawReferenceGround = true;
private:
	void DrawGeometryGameObjectButtons(GameObject* gameObjectParent);
	void DrawTreeNode(GameObject * gameObject);
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
	GameObject* gameObjectSelected = nullptr;
	std::list<GameObject*> gameObjectsToBeDeleted;
	std::vector<std::string>* fileSceneList = nullptr;
	GameObject* gameObjectToBeDuplicated = nullptr;
	int gameObjectCounter = 1;
	bool markToLoadScene = false;
};

#endif __MODULESCENE_H_
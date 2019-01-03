#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"

#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleLibrary.h"

#include "ComponentTransformation.h"
#include "ComponentCamera.h"

#include "Config.h"

#include <string>

char ModuleScene::sceneFileName[40] = {};
const char* ModuleScene::labelCurrentSceneFileName = "";
ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
	delete root;
	root = nullptr;
}

bool ModuleScene::Init()
{
	root = new GameObject("root", nullptr);

	fileSceneList = App->library->GetFileSceneList();

	return true;
}

update_status ModuleScene::Update()
{

	ManageDuplicationAndDeletionGameObject();

	if (markToLoadScene)
	{
		LoadScene();
		markToLoadScene = false;
	}

	if (markToUpdateSceneFiles)
	{
		fileSceneList = App->library->GetFileSceneList();
		labelCurrentSceneFileName = sceneFileName;
		markToUpdateSceneFiles = false;
	}

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent, bool withTransformation)
{
	GameObject* gameObject = new GameObject(name, parent);

	parent->childrens.push_back(gameObject);

	if (withTransformation)
	{
		float3 position = { 0.0f,0.0f,0.0f };
		float3 scale = { 1.0f,1.0f,1.0f };
		Quat quatRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);

		gameObject->components.push_back(new ComponentTransformation(gameObject, ComponentType::TRANSFORMATION, position, scale, quatRotation));
	}

	return gameObject;
}

void ModuleScene::CreateGameObject(Config* config, rapidjson::Value & value)
{
	if (value.HasMember("parent"))
	{
		const char* parentUuid = config->GetString("parent", value);
		char uuidGameObjectParent[37];
		sprintf_s(uuidGameObjectParent, parentUuid);
		GameObject* parent = GetGameObjectByUUID(root, uuidGameObjectParent);

		GameObject* gameObject = new GameObject(config->GetString("name", value), parent);

		gameObject->Load(config, value);

		parent->childrens.push_back(gameObject);
	}
	else
	{
		root->Load(config, value);
	}

}

void ModuleScene::DrawProperties()
{
	if (!ImGui::Begin("Game objects hierarchy", &toggleSceneProperties))
	{
		ImGui::End();
		return;
	}

	ImGui::Button("Scene options");

	if (ImGui::IsItemClicked(0))
	{
		ImGui::OpenPopup("SceneOptionsContextualMenu");
	}

	if (ImGui::BeginPopup("SceneOptionsContextualMenu"))
	{
		ImGui::Text("File name:");
		ImGui::SameLine();
		ImGui::InputText("##", sceneFileName, sizeof(sceneFileName));
		ImGui::SameLine();
		if (ImGui::Button("Save scene "))
		{
			SaveScene();
		}
		
		ImGui::Text("File name:");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##comboScene", labelCurrentSceneFileName))
		{
			for (std::vector<std::string>::iterator iterator = fileSceneList.begin(); iterator != fileSceneList.end(); ++iterator)
			{
				bool isSelected = (labelCurrentSceneFileName == (*iterator).c_str());
				if (ImGui::Selectable((*iterator).c_str(), isSelected))
				{
					labelCurrentSceneFileName = (*iterator).c_str();
					strcpy_s(sceneFileName, sizeof(sceneFileName), (*iterator).c_str());;
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load scene "))
		{
			if (root->childrens.size() > 0)
			{
				ClearScene();
				markToLoadScene = true;
			}
			else
			{
				LoadScene();
			}
		}
		if (ImGui::Button("Clear scene"))
		{
			ClearScene();
		}
		ImGui::Separator();
		if (ImGui::Button("Create generic game object"))
		{
			std::string genericGameObject = "genericGameObject." + std::to_string(gameObjectCounter++);
			CreateGameObject(genericGameObject.c_str(), root, true);
		}
		DrawGeometryGameObjectButtons(root);

		ImGui::Checkbox("Scene culling active", &isSceneCullingActive);

		ImGui::EndPopup();
	}

	ImGui::Separator();

	ImGui::SliderFloat("Ambient", &ambientLight, 0.0f, 1.0f);
	ImGui::Text("Light position:");
	ImGui::PushItemWidth(70);
	ImGui::Text("X:");
	ImGui::SameLine();
	ImGui::PushID("1");
	ImGui::DragFloat("", &ambientLightPosition.x, 10.0f, -100000.f, 100000.f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Y:");
	ImGui::SameLine();
	ImGui::PushID("2");
	ImGui::DragFloat("", &ambientLightPosition.y, 10.0f, -100000.f, 100000.f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Z:");
	ImGui::SameLine();
	ImGui::PushID("3");
	ImGui::DragFloat("", &ambientLightPosition.z, 10.0f, -100000.f, 100000.f);
	ImGui::PopID();
	ImGui::PopItemWidth();

	ImGui::Separator();

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (root->childrens.empty()) {
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool rootOpen = ImGui::TreeNodeEx(root->uuid, node_flags, root->name.c_str());

	if (rootOpen && !root->childrens.empty())
	{
		DragAndDropManagement(root);
		
		for (std::list<GameObject*>::iterator iterator = root->childrens.begin(); iterator != root->childrens.end(); ++iterator)
		{
			DrawTreeNode(*iterator);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleScene::DrawGeometryGameObjectButtons(GameObject* gameObjectParent)
{
	if (ImGui::Button("Sphere  "))
	{
		GameObject* gameObject = CreateGameObject("Sphere", gameObjectParent, false);
		App->modelLoader->LoadGeometry(gameObject, GeometryType::SPHERE);
	}
	if (ImGui::Button("Torus   "))
	{
		GameObject* gameObject = CreateGameObject("Torus", gameObjectParent, false);
		App->modelLoader->LoadGeometry(gameObject, GeometryType::TORUS);
	}
	if (ImGui::Button("Cylinder"))
	{
		GameObject* gameObject = CreateGameObject("Cylinder", gameObjectParent, false);
		App->modelLoader->LoadGeometry(gameObject, GeometryType::CYLINDER);
	}
	if (ImGui::Button("Cube    "))
	{
		GameObject* gameObject = CreateGameObject("Cube", gameObjectParent, false);
		App->modelLoader->LoadGeometry(gameObject, GeometryType::CUBE);
	}
	if (ImGui::Button("Plane   "))
	{
		GameObject* gameObject = CreateGameObject("Plane", gameObjectParent, false);
		App->modelLoader->LoadGeometry(gameObject, GeometryType::PLANE);
	}
}

void ModuleScene::DrawTreeNode(GameObject * gameObject)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (gameObject->childrens.empty())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (gameObject->isSelected)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool gameObjectOpen = ImGui::TreeNodeEx(gameObject->uuid, nodeFlags, gameObject->name.c_str());

	DragAndDropManagement(gameObject);
	ClickManagement(gameObject);

	if (gameObjectOpen)
	{
		for (std::list<GameObject*>::iterator iterator = gameObject->childrens.begin(); iterator != gameObject->childrens.end(); ++iterator)
		{
			DrawTreeNode(*iterator);
		}

		ImGui::TreePop();
	}
}

void ModuleScene::SetGameObjectSelected(GameObject * gameObject)
{
	if (gameObjectSelected != nullptr)
	{
		gameObjectSelected->isSelected = false;
	}

	gameObjectSelected = gameObject;
	gameObjectSelected->isSelected = true;
}

GameObject* ModuleScene::GetGameObjectByUUID(GameObject* gameObject, char uuidObjectName[37])
{
	GameObject* result = nullptr;

	if (result == nullptr && (strcmp(gameObject->uuid, uuidObjectName) == 0))
	{
		result = gameObject;
	}
	else
	{
		for (auto &gameObjectChild : gameObject->childrens)
		{
			if (gameObjectChild->childrens.size() > 0)
			{
				//TODO: change this to not use recursivity
				result = GetGameObjectByUUID(gameObjectChild, uuidObjectName);
			}

			if (result == nullptr && (strcmp(gameObjectChild->uuid, uuidObjectName) == 0))
			{
				result = gameObjectChild;
				break;
			}
			else if (result != nullptr)
			{
				break;
			}
		}
	}

	return result;
}

void ModuleScene::DragAndDropManagement(GameObject* gameObjectParent)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("uuidGameObject", gameObjectParent->uuid, 37);
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("uuidGameObject"))
		{
			char uuidGameObjectToMove[37];
			sprintf_s(uuidGameObjectToMove, (char*)payload->Data);

			GameObject* gameObjectToMove = GetGameObjectByUUID(root, uuidGameObjectToMove);

			if (gameObjectToMove != nullptr)
			{
				bool isOneGameObjectIsParentOfOther = CheckIfOneGameObjectIsParentOfOther(*gameObjectToMove, *gameObjectParent);

				if (!isOneGameObjectIsParentOfOther)
				{
					gameObjectToMove->parent->childrens.remove(gameObjectToMove);

					gameObjectToMove->parent = gameObjectParent;
					gameObjectParent->childrens.push_back(gameObjectToMove);
				}
				else
				{
					LOG("Is not possible to drop a gameObject 'parent' into one of its sons");
				}
			}
		}
	}
}

void ModuleScene::ClickManagement(GameObject* gameObject)
{
	if (ImGui::IsItemClicked(0))
	{
		SetGameObjectSelected(gameObject);
	}

	if (ImGui::IsItemClicked(1))
	{
		SetGameObjectSelected(gameObject);
		ImGui::OpenPopup("GameObjectContextualMenu");
	}

	if (gameObject->isSelected)
	{
		if (ImGui::BeginPopup("GameObjectContextualMenu"))
		{
			ImGui::Button("New game object");

			if (ImGui::IsItemClicked(0))
			{
				ImGui::OpenPopup("NewGameObjectsContextualMenu");
			}

			if (ImGui::BeginPopup("NewGameObjectsContextualMenu"))
			{
				if (ImGui::Button("Empty game object"))
				{
					std::string genericGameObject = "genericGameObject." + std::to_string(gameObjectCounter++);
					CreateGameObject(genericGameObject.c_str(), gameObject, true);
				}
				DrawGeometryGameObjectButtons(gameObject);

				ImGui::EndPopup();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Move up"))
			{
				MoveUpDownGameObject(gameObject, true);
			}
			if (ImGui::MenuItem("Move down"))
			{
				MoveUpDownGameObject(gameObject, false);
			}
			if (ImGui::MenuItem("Duplicate"))
			{
				gameObjectToBeDuplicated = gameObject;
			}
			if (ImGui::MenuItem("Delete"))
			{
				gameObjectsToBeDeleted.push_back(gameObject);
			}
			ImGui::EndPopup();
		}
	}
}

void ModuleScene::ManageDuplicationAndDeletionGameObject()
{

	if (gameObjectsToBeDeleted.size() > 0)
	{
		for (std::list<GameObject*>::iterator iterator = gameObjectsToBeDeleted.begin(); iterator != gameObjectsToBeDeleted.end(); ++iterator)
		{
			(*iterator)->RemoveGameObject(*iterator);
			RELEASE(*iterator);
			gameObjectSelected = nullptr;
		}

		gameObjectsToBeDeleted.clear();

	}

	if (gameObjectToBeDuplicated != nullptr)
	{
		gameObjectToBeDuplicated->DuplicateGameObject(nullptr);
		gameObjectToBeDuplicated = nullptr;
	}
}

bool ModuleScene::CheckIfOneGameObjectIsParentOfOther(const GameObject& gameObjectPossibleParent, const GameObject& gameObjectPossibleSon)
{
	bool result = false;

	if (gameObjectPossibleSon.parent != nullptr)
	{
		if (gameObjectPossibleSon.parent == &gameObjectPossibleParent)
		{
			result = true;
		}
		else
		{
			result = CheckIfOneGameObjectIsParentOfOther(gameObjectPossibleParent, *gameObjectPossibleSon.parent);
		}
	}

	return result;
}

void ModuleScene::MoveUpDownGameObject(GameObject * gameObject, bool up)
{
	std::list<GameObject*>::iterator iterator = gameObject->parent->childrens.begin();

	for (iterator; iterator != gameObject->parent->childrens.end(); ++iterator)
	{
		if ((*iterator)->uuid == gameObject->uuid)
		{
			break;
		}
	}

	if (up)
	{
		if (iterator != gameObject->parent->childrens.begin())
		{
			std::swap(*iterator, *std::prev(iterator));
		}
	}
	else
	{
		if (*iterator != gameObject->parent->childrens.back())
		{
			std::swap(*iterator, *std::next(iterator));
		}
	}
}

void ModuleScene::SaveScene()
{
	Config* config = new Config();
	config->StartObject("scene");

	config->AddFloat("ambientLight", ambientLight);
	config->AddFloat3("ambientLightPosition", ambientLightPosition);
	config->AddBool("isSceneCullingActive", isSceneCullingActive);

	config->EndObject();

	config->AddName("sceneCamera");
	App->camera->sceneCamera->Save(config);

	if (App->camera->selectedCamera != nullptr)
	{
		config->AddName("selectedCamera");
		App->camera->selectedCamera->Save(config);
	}

	config->StartArray("gameObjects");
	SaveGameObject(config, root);
	config->EndArray();

	std::string fullPathSceneFileName = sceneFileName;
	fullPathSceneFileName.insert(0, "/Library/Scene/");
	config->WriteToDisk(fullPathSceneFileName.c_str());

	App->library->UpdateSceneList();
	markToUpdateSceneFiles = true;
}

void ModuleScene::SaveGameObject(Config* config, GameObject* gameObject)
{
	gameObject->Save(config);

	if (gameObject->childrens.size() > 0)
	{
		//TODO: change this to not use recursivity
		for (std::list<GameObject*>::iterator iterator = gameObject->childrens.begin(); iterator != gameObject->childrens.end(); ++iterator)
		{
			SaveGameObject(config, (*iterator));
		}
	}
}

void ModuleScene::LoadScene()
{
	Config* config = new Config();

	std::string fullPathSceneFileName = sceneFileName;
	fullPathSceneFileName.insert(0, "/Library/Scene/");
	rapidjson::Document document = config->LoadFromDisk(fullPathSceneFileName.c_str());

	if (!document.HasParseError())
	{
		rapidjson::Value& scene = document["scene"];

		ambientLight = config->GetFloat("ambientLight", scene);
		ambientLightPosition = config->GetFloat3("ambientLightPosition", scene);
		isSceneCullingActive = config->GetBool("isSceneCullingActive", scene);

		App->camera->sceneCamera->Load(config, document["sceneCamera"]);

		rapidjson::Value gameObjects = document["gameObjects"].GetArray();
		for (rapidjson::Value::ValueIterator it = gameObjects.Begin(); it != gameObjects.End(); it++)
		{
			CreateGameObject(config, *it);
		}

		if (document.HasMember("selectedCamera"))
		{
			rapidjson::Value& selectedCamera = document["selectedCamera"];
			if (selectedCamera.HasMember("gameObjectParent"))
			{
				const char* parentUuid = config->GetString("gameObjectParent", selectedCamera);
				char uuidGameObjectParent[37];
				sprintf_s(uuidGameObjectParent, parentUuid);
				GameObject* gameObjecteCameraSelected = GetGameObjectByUUID(root, uuidGameObjectParent);

				Component* camera = gameObjecteCameraSelected->GetComponent(ComponentType::CAMERA);

				App->renderer->componentCameraGameSelected = (ComponentCamera*)camera;
			}
		}
	}
}

void ModuleScene::ClearScene()
{
	gameObjectsToBeDeleted = root->childrens;
}

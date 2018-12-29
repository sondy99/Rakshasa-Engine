#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"

#include "ModuleModelLoader.h"
#include "ModuleTextures.h"

#include "ComponentTransformation.h"
#include "ComponentCamera.h"

#include <string>

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
	
	//LoadModel("Assets/ZomBunny/Zombunny.fbx");
	//LoadModel("Assets/BakerHouse/BakerHouse.fbx");

	return true;
}

void ModuleScene::LoadModel(const char * modelPath)
{
	GameObject* gameObject = CreateGameObject("gameObject", root, false);

	App->modelLoader->LoadMaterialFromFBX(modelPath, gameObject);
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

	ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
	ImGui::Text("Light position:");
	ImGui::PushItemWidth(70);
	ImGui::Text("X:");
	ImGui::SameLine();
	ImGui::PushID("1");
	ImGui::DragFloat("", &lightPosition.x, 10.0f, -100000.f, 100000.f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Y:");
	ImGui::SameLine();
	ImGui::PushID("2");
	ImGui::DragFloat("", &lightPosition.y, 10.0f, -100000.f, 100000.f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Z:");
	ImGui::SameLine();
	ImGui::PushID("3");
	ImGui::DragFloat("", &lightPosition.z, 10.0f, -100000.f, 100000.f);
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

		ManageDuplicationAndDeletionGameObject();

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
				gameObjectToBeDeleted = gameObject;
			}
			ImGui::EndPopup();
		}
	}
}

void ModuleScene::ManageDuplicationAndDeletionGameObject()
{

	if (gameObjectToBeDeleted != nullptr)
	{
		gameObjectToBeDeleted->RemoveGameObject(gameObjectToBeDeleted);
		gameObjectToBeDeleted = nullptr;
		gameObjectSelected = nullptr;
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
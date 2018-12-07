#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"

#include "ModuleModelLoader.h"

#include "ComponentTransformation.h"

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
			
	LoadModel("BakerHouse.FBX");

	return true;
}

void ModuleScene::LoadModel(const char * modelPath)
{
	GameObject* gameObject = CreateGameObject("gameObject", root, false);

	App->modelLoader->Load(modelPath, gameObject);
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent, bool withTransformation)
{
	GameObject* gameObject = new GameObject(name, parent);

	if(parent == root)
	{
		root->childrens.push_back(gameObject);
	}

	if (withTransformation)
	{
		float3 position = { 0.0f,0.0f,0.0f };
		float3 scale = { 1.0f,1.0f,1.0f };
		Quat quatRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);

		gameObject->components.push_back(new ComponentTransformation(parent, ComponentType::TRANSFORMATION, position, scale, quatRotation));
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

	if (ImGui::Button("Transform to model identity"))
	{
		CreateGameObject("genericGameObject", root, true);
	}

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (root->childrens.empty()) {
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool rootOpen = ImGui::TreeNodeEx(root->uuid, node_flags, root->name.c_str());

	if (rootOpen && !root->childrens.empty())
	{
		DragAndDropManagement(root);

		if (gameObjectToBedeleted != nullptr)
		{
			gameObjectToBedeleted->remove();
			gameObjectToBedeleted = nullptr;
			gameObjectSelected = nullptr;
		}

		for (std::list<GameObject*>::iterator iterator = root->childrens.begin(); iterator != root->childrens.end(); ++iterator)
		{
			DrawTreeNode(*iterator);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleScene::DrawTreeNode(GameObject * gameObject)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (gameObject->childrens.empty()) {
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
				gameObjectToMove->parent->childrens.remove(gameObjectToMove);

				gameObjectToMove->parent = gameObjectParent;
				gameObjectParent->childrens.push_back(gameObjectToMove);
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
			if (ImGui::MenuItem("Duplicate"))
			{

			}
			if (ImGui::MenuItem("Delete"))
			{
				gameObjectToBedeleted = gameObject;
			}
			ImGui::EndPopup();
		}
	}
}

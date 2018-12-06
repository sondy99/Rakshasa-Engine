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

	DragAndDropManagement(root);

	if (rootOpen)
	{
		for (auto &gameObjectChild : root->childrens) 
		{
			DrawTreeNode(gameObjectChild);
		}
		ImGui::TreePop();
	}

	ImGui::End();
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

void ModuleScene::DrawTreeNode(GameObject * gameObjectParent)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (root->childrens.empty()) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (gameObjectParent->isSelected)
	{
		nodeFlags = ImGuiTreeNodeFlags_Selected;
	}

	bool gameObjectOpen = ImGui::TreeNodeEx(gameObjectParent->uuid, nodeFlags, gameObjectParent->name.c_str());
	
	if (ImGui::IsItemClicked(0))
	{
		SetGameObjectSelected(gameObjectParent);
	}

	DragAndDropManagement(gameObjectParent);

	if (gameObjectOpen)
	{
		for (auto &gameObjectChild : gameObjectParent->childrens)
		{
			DrawTreeNode(gameObjectChild);
		}

		ImGui::TreePop();
	}}

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

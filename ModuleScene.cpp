#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"

#include "ModuleModelLoader.h"

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

	GameObject* nivelUno = new GameObject("primer nivel uno", root);
	GameObject* subnivelTres = new GameObject("sub nivel tres", nivelUno);
	nivelUno->childrens.push_back(subnivelTres);

	root->childrens.push_back(nivelUno);

	GameObject* nivelDos = new GameObject("primer nivel dos", root);
	GameObject* subnivelUno = new GameObject("sub nivel uno", nivelDos);
	nivelDos->childrens.push_back(subnivelUno);
	GameObject* subnivelDos = new GameObject("sub nivel dos", nivelDos);
	nivelDos->childrens.push_back(subnivelDos);
	GameObject* subsubniveluno = new GameObject("subsub nivel uno", subnivelDos);
	subnivelDos->childrens.push_back(subsubniveluno);

	root->childrens.push_back(nivelDos);

	GameObject* nivelTres = new GameObject("primer nivel tres", root);
	root->childrens.push_back(nivelTres);
		
	LoadModel("BakerHouse.FBX");

	return true;
}

void ModuleScene::LoadModel(const char * modelPath)
{
	GameObject* gameObject = CreateGameObject("gameObject", nullptr);

	App->modelLoader->Load(modelPath, gameObject);
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObjectParent = new GameObject(name, parent);

	if(parent == nullptr)
	{
		root->childrens.push_back(gameObjectParent);
	}

	return gameObjectParent;
}

void ModuleScene::DrawProperties() 
{
	if (!ImGui::Begin("Game objects hierarchy", &toggleSceneProperties)) 
	{
		ImGui::End();
		return;
	}

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (root->childrens.empty()) {
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool rootOpen = ImGui::TreeNodeEx(root->uuid, node_flags, root->name.c_str());

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

			GameObject* gameObjectToMove = GetGameObjectByName(root, uuidGameObjectToMove);

			if (gameObjectToMove != nullptr)
			{
				std::vector<GameObject*>::iterator position = std::find(gameObjectToMove->parent->childrens.begin(), gameObjectToMove->parent->childrens.end(), gameObjectToMove);

				if (position != gameObjectToMove->parent->childrens.end())
				{
					gameObjectToMove->parent->childrens.erase(position);
				}

				gameObjectToMove->parent = gameObjectParent;
				gameObjectParent->childrens.push_back(gameObjectToMove);
			}
		}
	}

	if (gameObjectOpen)
	{
		for (auto &gameObjectChild : gameObjectParent->childrens)
		{
			DrawTreeNode(gameObjectChild);
		}

		ImGui::TreePop();
	}}

GameObject* ModuleScene::GetGameObjectByName(GameObject* gameObject, char uuidObjectName[37])
{
	GameObject* result = nullptr;

	for (auto &gameObjectChild : gameObject->childrens)
	{
		if (gameObjectChild->childrens.size() > 0)
		{
			//TODO: change this to not use recursivity
			result = GetGameObjectByName(gameObjectChild, uuidObjectName);
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
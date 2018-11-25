#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"

ModuleScene::ModuleScene()
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
}

ModuleScene::~ModuleScene()
{
	delete root;
	root = nullptr;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = new GameObject(name, parent);

	root->childrens.push_back(gameObject);

	return gameObject;
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

	bool rootOpen = ImGui::TreeNodeEx(root->uuid.c_str(), node_flags, root->name);

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

void ModuleScene::DrawTreeNode(GameObject * gameObject)
{

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (root->childrens.empty()) {
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool gameObjectOpen = ImGui::TreeNodeEx(gameObject->uuid.c_str(), node_flags, gameObject->name);

	if (gameObjectOpen)
	{
		for (auto &gameObjectChild : gameObject->childrens)
		{
			DrawTreeNode(gameObjectChild);
		}

		ImGui::TreePop();
	}
}

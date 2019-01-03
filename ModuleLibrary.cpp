#include "ModuleLibrary.h"

#include "Application.h"

#include "ModuleFileSystem.h"

#include "ImporterMaterial.h"
#include "ImporterMesh.h"

#include "thread";

bool stopWatcher = false;
void LibraryWatcher()
{
	std::map<std::string, std::string> oldFilesAssets;
	std::map<std::string, std::string> currentFilesAssets;
	std::map<std::string, std::string> currentFilesLibrary;
	while (!stopWatcher)
	{
		currentFilesAssets = App->fileSystem->GetFilesFromDirectoryRecursive("/Assets/", true);
		if ((oldFilesAssets.size() == 0 && oldFilesAssets.size() != currentFilesAssets.size()) 
			|| oldFilesAssets.size() < currentFilesAssets.size())
		{
			currentFilesLibrary = App->fileSystem->GetFilesFromDirectoryRecursive("/Library/", false);
			for (std::map<std::string, std::string>::iterator iterator = currentFilesAssets.begin(); iterator != currentFilesAssets.end(); ++iterator)
			{
				std::string fileName = (*iterator).first;
				App->fileSystem->ChangePathSlashes(fileName);
				if (currentFilesLibrary.find(fileName.substr(0, fileName.length() - 4)) == currentFilesLibrary.end())
				{
					std::string ext(fileName.substr(fileName.length() - 3));

					std::string fullPath = (*iterator).second;
					fullPath.append(fileName);
					if (ext == "png" || ext == "tif")
					{
						ImporterMaterial::Import(fullPath.c_str());
					}
					if (ext == "fbx" || ext == "FBX")
					{
						ImporterMesh::ImportFBX(fullPath.c_str());
						
					}
				}
			}
			oldFilesAssets = currentFilesAssets;
			App->library->UpdateMeshesList();
			App->library->UpdateTexturesList();
		}
		else if (oldFilesAssets.size() > currentFilesAssets.size())
		{
			oldFilesAssets = currentFilesAssets;
		} 
	}

	Sleep(1000);
}

ModuleLibrary::ModuleLibrary()
{
}

ModuleLibrary::~ModuleLibrary()
{
}

bool ModuleLibrary::Init()
{
	std::thread watcherThread(LibraryWatcher);

	watcherThread.detach();

	UpdateMeshesList();
	UpdateTexturesList();
	UpdateSceneList();

	return true;
}

update_status ModuleLibrary::Update()
{
	if (resourceMarkToBeDeleted)
	{
		resourceMarkToBeDeleted = false;
		if (removeChamo)
		{
			UpdateMeshesList();
			removeChamo = false;
		}
		else if (removeTexture)
		{
			UpdateTexturesList();
			removeTexture = false;
		}
		else if (removeScene) 
		{
			UpdateSceneList();
			removeScene = false;
		}
	}

	return UPDATE_CONTINUE;
}

bool ModuleLibrary::CleanUp()
{
	stopWatcher = true;
	return true;
}

void ModuleLibrary::DrawProperties()
{
	if (!ImGui::Begin("Library properties", &toggleLibraryProperties))
	{
		ImGui::End();
		return;
	}

	ImGui::Button("Library options");

	if (ImGui::IsItemClicked(0))
	{
		ImGui::OpenPopup("LibraryOptionsContextualMenu");
	}

	if (ImGui::BeginPopup("LibraryOptionsContextualMenu"))
	{
		if (ImGui::Button("Some options"))
		{
		}

		ImGui::EndPopup();
	}

	ImGui::Separator();
	
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	
	bool libraryOpen = ImGui::TreeNodeEx("Library", node_flags, "Library");

	if (libraryOpen)
	{
		DrawTreeNode("Meshes", false);
		DrawTreeNode("Textures", false);
		DrawTreeNode("Scene", false);

		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleLibrary::UpdateMeshesList()
{
	fileMeshesList.clear();
	App->fileSystem->GetFilesFromDirectory("/Library/Meshes/", fileMeshesList);
}

void ModuleLibrary::UpdateTexturesList()
{
	fileTexturesList.clear();
	App->fileSystem->GetFilesFromDirectory("/Library/Textures/", fileTexturesList);
}

void ModuleLibrary::UpdateSceneList()
{
	fileSceneList.clear();
	App->fileSystem->GetFilesFromDirectory("/Library/Scene/", fileSceneList);
}

void ModuleLibrary::DrawTreeNode(const char* name, bool isLeaf)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (isLeaf) 
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (itemSelected == name)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool resourceOpen = ImGui::TreeNodeEx(name, nodeFlags, name);

	ClickManagement(name);
	
	if (resourceOpen)
	{
		if (name == "Meshes")
		{
			for (std::vector<std::string>::iterator iterator = fileMeshesList.begin(); iterator != fileMeshesList.end(); ++iterator)
			{
				DrawTreeNode((*iterator).c_str(), true);
			}
		}
		else if (name == "Textures")
		{
			for (std::vector<std::string>::iterator iterator = fileTexturesList.begin(); iterator != fileTexturesList.end(); ++iterator)
			{
				DrawTreeNode((*iterator).c_str(), true);
			}
		}
		else if (name == "Scene")
		{
			for (std::vector<std::string>::iterator iterator = fileSceneList.begin(); iterator != fileSceneList.end(); ++iterator)
			{
				DrawTreeNode((*iterator).c_str(), true);
			}
		}

		ImGui::TreePop();
	}
}

void ModuleLibrary::ClickManagement(const char * name)
{
	if (ImGui::IsItemClicked(0))
	{
		itemSelected = name;
	}

	if (ImGui::IsItemClicked(1))
	{
		itemSelected = name;
		ImGui::OpenPopup("ResourceObjectContextualMenu");
	}

	if (itemSelected == name)
	{
		if (ImGui::BeginPopup("ResourceObjectContextualMenu"))
		{

			if (ImGui::MenuItem("Delete"))
			{
				std::string ext;
				App->fileSystem->SplitFilePath(name, nullptr, nullptr, &ext);
				std::string nameToRemove = name;
				if (ext == "chamo")
				{
					nameToRemove.insert(0, "/Library/Meshes/");
					removeChamo = true;
					resourceMarkToBeDeleted = true;
				}
				else if (ext == "dds")
				{
					nameToRemove.insert(0, "/Library/Textures/");
					removeTexture = true;
					resourceMarkToBeDeleted = true;
				}
				else if (ext == "json")
				{
					nameToRemove.insert(0, "/Library/Scene/");
					removeScene = true;
					resourceMarkToBeDeleted = true;
				}

				App->fileSystem->Remove(nameToRemove.c_str());
			}
			ImGui::EndPopup();
		}
	}
}

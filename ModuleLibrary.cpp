#include "ModuleLibrary.h"

#include "Application.h"

#include "ModuleFileSystem.h"

#include "ImporterMaterial.h"

#include "thread";

bool stopWatcher = false;
void LibraryWatcher()
{
	std::map<std::string, std::string> currentFiles;
	std::map<std::string, std::string> files;
	while (!stopWatcher)
	{
		currentFiles = App->fileSystem->GetFilesFromDirectoryRecursive("/Library/");
		files = App->fileSystem->GetFilesFromDirectoryRecursive("/Assets/");
		if (files.size() != currentFiles.size())
		{
			for (std::map<std::string, std::string>::iterator iterator = files.begin(); iterator != files.end(); ++iterator)
			{
				if (currentFiles.find((*iterator).first) == currentFiles.end())
				{
					std::string ext((*iterator).first.substr((*iterator).first.length() - 3));

					if (ext == "png" || ext == "tif")
					{
						std::string fullPath = (*iterator).second;
						fullPath.append((*iterator).first);
						if (ImporterMaterial::Import(fullPath.c_str()))
						{
							currentFiles.insert(*iterator);
						}
					}
				}
			}

			App->library->UpdateDirectoryAndFileList();
		}
	}

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

	UpdateDirectoryAndFileList();

	return true;
}

update_status ModuleLibrary::Update()
{	
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

		ImGui::TreePop();
	}

	ImGui::End();
}

void ModuleLibrary::UpdateDirectoryAndFileList()
{
	fileMeshList.clear();
	fileTexturesList.clear();
	App->fileSystem->GetFilesFromDirectory("/Library/Meshes/", fileMeshList);
	App->fileSystem->GetFilesFromDirectory("/Library/Textures/", fileTexturesList);
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
			for (std::vector<std::string>::iterator iterator = fileMeshList.begin(); iterator != fileMeshList.end(); ++iterator)
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
				}
				else if (ext == "dds")
				{
					nameToRemove.insert(0, "/Library/Textures/");
				}

				App->fileSystem->Remove(nameToRemove.c_str());
			}
			ImGui::EndPopup();
		}
	}
}

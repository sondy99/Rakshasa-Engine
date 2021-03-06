#ifndef __MODULELIBRARY_H__
#define __MODULELIBRARY_H__

#include "Module.h"

#include "map"
#include "vector"
#include "string"

class ModuleLibrary : public Module
{
public:
	ModuleLibrary();
	~ModuleLibrary();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void DrawProperties();
	void UpdateMeshesList();
	void UpdateTexturesList();
	void UpdateSceneList();
	inline std::vector<std::string>* GetFileMeshList() const { return fileMeshesList; }
	inline std::vector<std::string>* GetFileTexturesList() const { return fileTexturesList; }
	inline std::vector<std::string>* GetFileSceneList() const { return fileSceneList; }
public:
	bool toggleLibraryProperties = true;
private:
	void DrawTreeNode(const char* name, bool isLeaf);
	void ClickManagement(const char* name);
private:
	bool resourceMarkToBeDeleted = false;
	bool removeChamo = false;
	bool removeTexture = false;
	bool removeScene = false;
	std::string itemSelected;
	std::vector<std::string>* fileMeshesList;
	std::vector<std::string>* fileTexturesList;
	std::vector<std::string>* fileSceneList;
};

#endif __MODULELIBRARY_H__

#ifndef __ModuleLibrary_h__
#define __ModuleLibrary_h__

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
	inline std::vector<std::string> GetFileMeshList() const { return fileMeshesList; }
	inline std::vector<std::string> GetFileTexturesList() const { return fileTexturesList; }
public:
	bool toggleLibraryProperties = true;
private:
	void DrawTreeNode(const char* name, bool isLeaf);
	void ClickManagement(const char* name);
private:
	bool resourceMarkToBeDeleted = false;
	bool removeChamo = false;
	std::string itemSelected;
	std::vector<std::string> fileMeshesList;
	std::vector<std::string> fileTexturesList;
};

#endif // __ModuleLibrary_h__

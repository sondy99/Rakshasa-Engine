#ifndef __ModuleLibrary_h__
#define __ModuleLibrary_h__

#include "Module.h"

#include "map"
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
public:
	bool toggleLibraryProperties = true;
private:
	void DrawTreeNode(const char* name, bool isLeaf);
	void ClickManagement(const char* name);
private:
	std::string itemSelected;
	std::map<std::string, std::string> currentFiles;
};

#endif // __ModuleLibrary_h__

#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"

#include "map"
#include "vector"
#include "string"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	bool AddPath(const char* path);
	unsigned int Load(const char* pathAndFileName, char** buffer) const;
	unsigned int Save(const char* pathAndFileName, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* pathAndFileName);
	bool Exists(const char* pathAndFileName) const;
	bool MakeDirectory(const char* path);
	bool IsDirectory(const char* pathAndFileName) const;
	bool Copy(const char * sourcePath, const char * destinationPath);
	void GetFilesFromDirectoryRecursive(const char* directory, const bool withExtension, std::map<std::string, std::string>& result);
	void GetFilesFromDirectory(const char* directory, std::vector<std::string>& fileList) const;
	void ChangePathSlashes(std::string & fullPath) const;
	void SplitFilePath(const char* fullPath, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;
public:
	char* basePath = nullptr;
};

#endif __MODULEFILESYSTEM_H__

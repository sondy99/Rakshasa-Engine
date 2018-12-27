#include "ModuleFileSystem.h"

#include "Globals.h"
#include "Application.h"

#include "SDL.h"
#include "physfs.h"

ModuleFileSystem::ModuleFileSystem()
{
}

ModuleFileSystem::~ModuleFileSystem()
{
}

bool ModuleFileSystem::Init()
{
	basePath = SDL_GetPrefPath("Rakshasa", "");
	PHYSFS_init(basePath);
	SDL_free(basePath);

	AddPath(".");

	AddPath("D:/Proyectos/Rakshasa-Engine/Game/");

	PHYSFS_setWriteDir(".");

	if (PHYSFS_exists("/Library/") == 0)
		PHYSFS_mkdir("/Library/");
	if (PHYSFS_exists("/Library/Textures/") == 0)
		PHYSFS_mkdir("/Library/Textures/");
	if (PHYSFS_exists("/Library/Meshes/") == 0)
		PHYSFS_mkdir("/Library/Meshes/");
	return true;
}

update_status ModuleFileSystem::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleFileSystem::CleanUp()
{
	PHYSFS_deinit();
	return true;
}

unsigned int ModuleFileSystem::Load(const char * pathAndFileName, char ** buffer) const
{
	unsigned result = 0;

	PHYSFS_file* fsFile = PHYSFS_openRead(pathAndFileName);

	if (fsFile != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fsFile);

		if (size > 0)
		{
			*buffer = new char[size];
			unsigned readed = (unsigned)PHYSFS_read(fsFile, *buffer, 1, size);
			if (readed != size)
			{
				LOG("Error reading from file %s: %s\n", pathAndFileName, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
			{
				result = readed;
			}
		}

		if (PHYSFS_close(fsFile) == 0)
		{
			LOG("Error closing file %s: %s\n", pathAndFileName, PHYSFS_getLastError());
		}
	}
	else
	{
		const char* error = PHYSFS_getLastError();
		LOG("Error opening file %s: %s\n", pathAndFileName, error);
	}

	return result;
}

unsigned int ModuleFileSystem::Save(const char * pathAndFileName, const void * buffer, unsigned int size, bool append) const
{
	unsigned result = 0;

	bool overwrite = PHYSFS_exists(pathAndFileName) != 0;
	PHYSFS_file* fsFile = (append) ? PHYSFS_openAppend(pathAndFileName) : PHYSFS_openWrite(pathAndFileName);

	if (fsFile != nullptr)
	{
		unsigned written = (unsigned)PHYSFS_write(fsFile, (const void*)buffer, 1, size);
		if (written == size)
		{
			result = written;
		}
		else
		{
			LOG("Error while writing to file %s: %s", pathAndFileName, PHYSFS_getLastError());
		}

		if (PHYSFS_close(fsFile) == 0)
		{
			LOG("Error closing file %s: %s", pathAndFileName, PHYSFS_getLastError());
		}
	}
	else
	{
		LOG("Error opening file %s: %s", pathAndFileName, PHYSFS_getLastError());
	}

	return result;
}

bool ModuleFileSystem::Remove(const char * pathAndFileName)
{
	bool result = false;

	if (pathAndFileName != nullptr)
	{
		if (PHYSFS_delete(pathAndFileName) == 0)
		{
			LOG("File deleted: [%s]", pathAndFileName);
			result = true;
		}
		else
		{
			LOG("Error while trying to delete [%s]: ", pathAndFileName, PHYSFS_getLastError());
		}
	}

	return result;
}

bool ModuleFileSystem::Exists(const char * pathAndFileName) const
{
	return PHYSFS_exists(pathAndFileName) != 0;
}

bool ModuleFileSystem::MakeDirectory(const char * path)
{


	return PHYSFS_mkdir(path) != 0;
}

bool ModuleFileSystem::AddPath(const char* path)
{
	bool result = false;

	if (PHYSFS_mount(path, nullptr, 1) == 0)
	{
		LOG("Error adding a path: %s\n", PHYSFS_getLastError());
	}
	else
	{
		result = true;
	}

	return result;
}

bool ModuleFileSystem::IsDirectory(const char * pathAndFileName) const
{
	return PHYSFS_isDirectory(pathAndFileName) != 0;
}

bool ModuleFileSystem::Copy(const char * sourcePath, const char * destinationPath)
{
	bool result = false;

	char buffer[8192];

	PHYSFS_file* source = PHYSFS_openRead(sourcePath);
	PHYSFS_file* destination = PHYSFS_openWrite(destinationPath);

	PHYSFS_sint32 size;
	if (source && destination)
	{
		while (size = (PHYSFS_sint32)PHYSFS_read(source, buffer, 1, 8192))
		{
			PHYSFS_write(destination, buffer, 1, size);
		}

		PHYSFS_close(source);
		PHYSFS_close(destination);
		result = true;

		LOG("Copied file [%s] to [%s]", sourcePath, destinationPath);
	}
	else
	{
		LOG("Error while copy from [%s] to [%s]", sourcePath, destinationPath);
	}

	return result;
}

void ModuleFileSystem::GetFilesAndDirectoriesFromPath(const char * directory, std::vector<std::string>& fileList, std::vector<std::string>& directoryList) const
{
	char **enumeratedFIles = PHYSFS_enumerateFiles(directory);
	char **iterator;

	std::string dir(directory);

	for (iterator = enumeratedFIles; *iterator != nullptr; iterator++)
	{
		if (PHYSFS_isDirectory((dir + *iterator).c_str()))
		{
			directoryList.push_back(*iterator);
		}
		else
		{
			fileList.push_back(*iterator);
		}
	}

	PHYSFS_freeList(enumeratedFIles);
}

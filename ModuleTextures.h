#ifndef __ModuleTextures_H__
#define __ModuleTextures_H__

#include<list>
#include "Module.h"
#include "ModuleModelLoader.h"
#include "Globals.h"
#include "GL/glew.h"

struct SDL_Texture;

struct Mesh
{
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned verticesNumber = 0;
	unsigned indicesNumber = 0;
};

struct Material
{
	int width = 0;
	int height = 0;
	unsigned texture0 = 0;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;

	Material Load(const char* path);
	void Unload(unsigned id);

};

#endif __ModuleTextures_H__
#ifndef __ModuleTextures_H__
#define __ModuleTextures_H__

#include<list>
#include "Module.h"
#include "ModuleModelLoader.h"
#include "Globals.h"
#include "GL/glew.h"

#include "string"

struct SDL_Texture;
struct aiMesh;
struct aiMaterial;

struct Mesh
{
	unsigned vao = 0;
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned verticesNumber = 0;
	unsigned indicesNumber = 0;
	unsigned normalsOffset = 0;
	unsigned texturesOffset = 0;
	unsigned vertexSize = 0;
	aiMesh* mesh = nullptr;
};

struct Material
{
	int width = 0;
	int height = 0;
	unsigned texture0 = 0;
	aiMaterial* material = nullptr;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;

	Material Load(std::string path);
	void Unload(unsigned id);

};

#endif __ModuleTextures_H__
#ifndef __ModuleModelLoader_H__
#define __ModuleModelLoader_H__

#include "Module.h"
#include "MathGeoLib.h"
#include <vector>

struct aiScene;

class Mesh
{
public:
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned verticesNumber = 0;
	unsigned indicesNumber = 0;
};

class Material
{
public:
	Material(int texture0, int width, int height) : texture0(texture0), width(width), height(height) 
	{
	}

	int width = 0;
	int height = 0;
	unsigned texture0 = 0;
};

//code base on Carlos's code
class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init() override;
	bool CleanUp() override;

	void Load(const char * filePath);
	void DrawProperties();
private:
	void GenerateMeshes(const aiScene* scene);
	void GenerateMaterials(const aiScene* scene);
public:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	math::float4x4 transform = math::float4x4::identity;
};

#endif __ModuleModelLoader_H__
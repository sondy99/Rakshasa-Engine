#ifndef __ModuleModelLoader_H__
#define __ModuleModelLoader_H__

#include "Module.h"
#include "MathGeoLib.h"
#include <vector>

struct aiScene;

//code base on Carlos's code
class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init() override;
	bool CleanUp() override;
private:
	void GenerateMeshes(const aiScene* scene);
	void GenerateMaterials(const aiScene* scene);
public:
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
		unsigned texture0 = 0;
	};

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	math::float4x4 transform = math::float4x4::identity;
};

#endif __ModuleModelLoader_H__
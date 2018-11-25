#ifndef __ModuleModelLoader_H__
#define __ModuleModelLoader_H__

#include "Module.h"

#include "MathGeoLib.h"

#include <vector>

struct aiScene;

class GameObject;
class ComponentMesh;
class ComponentMaterial;

struct Mesh;
struct Material;

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init() override;
	bool CleanUp() override;

	void Load(const char * filePath, GameObject* gameObject);
	void DrawProperties();
public:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	math::float4x4 transform = math::float4x4::identity;

	bool toggleModelProperties = true;
private:
	void GenerateMeshes(const aiScene* scene, GameObject* gameObject);
	void GenerateMaterials(const aiScene* scene, GameObject* gameObject);
};

#endif __ModuleModelLoader_H__
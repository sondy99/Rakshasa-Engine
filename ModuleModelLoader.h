#ifndef __ModuleModelLoader_H__
#define __ModuleModelLoader_H__

#include "Module.h"

#include "MathGeoLib.h"

#include <vector>

struct aiScene;
struct aiNode;
struct aiMesh;

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

	void Load(const char * filePath, GameObject* gameObjectParent);
	void DrawProperties();
public:
	math::float4x4 transform = math::float4x4::identity;

	bool toggleModelProperties = true;
private:
	void CleanUpMeshesAndTextures(const GameObject* gameObject);
	void GenerateMesh(Mesh& meshStruct);
	void GenerateMaterial(Material& materialStruct);
	void CreateGameObjectsFromNode(const aiScene* scene, const aiNode* node, GameObject* gameObjectParent);
};

#endif __ModuleModelLoader_H__
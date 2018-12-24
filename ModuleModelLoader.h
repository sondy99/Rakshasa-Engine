#ifndef __ModuleModelLoader_H__
#define __ModuleModelLoader_H__

#include "Module.h"

#include "MathGeoLib.h"

#include <vector>

struct aiScene;
struct aiNode;
struct aiMesh;
struct par_shapes_mesh_s;

class GameObject;
class ComponentMesh;
class ComponentMaterial;

struct Mesh;
struct Material;

enum class GeometryType
{
	SPHERE,
	TORUS,
	CYLINDER,
	PLANE,
	CUBE
};

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init() override;
	bool CleanUp() override;

	void LoadMaterialFromFBX(const char * filePath, GameObject* gameObjectParent);
	void LoadGeometry(GameObject* gameObjectParent, GeometryType geometryType, const math::float4& color);
	void DrawProperties();
public:
	math::float4x4 transform = math::float4x4::identity;

	bool toggleModelProperties = false;
private:
	void CleanUpMeshesAndTextures(const GameObject* gameObject);
	void GenerateMesh(Mesh& meshStruct, aiMesh* mesh);
	void GenerateMesh(const par_shapes_mesh_s* parShapeMesh, Mesh& meshStruct);
	void GenerateMaterial(Material& materialStruct);
	void CreateGameObjectsFromNode(const aiScene* scene, const aiNode* node ,GameObject* gameObjectParent);
	void CreateMeshComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh);
	void CreateMeshComponent(const par_shapes_mesh_s* parShapeMesh, GameObject* gameObjectMesh, const math::float4& color);
	void CreateMaterialComponent(GameObject* gameObjectMesh, const math::float4& color);
	void CreateMaterialComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh, unsigned materialIndex);
	void CreateTransformationComponent(GameObject* gameObject);
	void CreateTransformationComponent(const aiNode* node, GameObject* gameObject);
	void GenerateVAO(Mesh& mesh);
};

#endif __ModuleModelLoader_H__
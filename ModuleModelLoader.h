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

	void CleanUpMesh(Mesh meshStruct);
	void GenerateMesh(Mesh& meshStruct);

	void LoadGeometry(GameObject* gameObjectParent, GeometryType geometryType);
	void DrawProperties();
public:
	math::float4x4 transform = math::float4x4::identity;

	bool toggleModelProperties = false;
private:
	void CleanUpMeshesAndTextures(const GameObject* gameObject);
	void GenerateMesh(const par_shapes_mesh_s* parShapeMesh, Mesh& meshStruct);
	void CreateMeshComponent(const par_shapes_mesh_s* parShapeMesh, GameObject* gameObjectMesh, const math::float4& color);
	void CreateMaterialComponent(GameObject* gameObjectMesh, const math::float4& color);
	void CreateTransformationComponent(GameObject* gameObject);
	void GenerateVAO(Mesh& mesh);
};

#endif __ModuleModelLoader_H__
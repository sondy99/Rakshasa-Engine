#ifndef __MODULEMODELLOADER_H__
#define __MODULEMODELLOADER_H__

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
	NONE,
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

	void CleanUpMesh(Mesh* meshStruct);
	void GenerateMesh(Mesh* meshStruct);
	void GenerateMesh(const par_shapes_mesh_s* parShapeMesh, Mesh* meshStruct);

	void LoadGeometry(GameObject* gameObjectParent, GeometryType geometryType);
	void DrawProperties();
	par_shapes_mesh_s* CreateParShapesByGeometryType(GeometryType geometryType);
	void FreeParShapes(par_shapes_mesh_s* parMesh);
public:
	math::float4x4 transform = math::float4x4::identity;

	bool toggleModelProperties = false;
private:
	void CreateMeshComponent(const par_shapes_mesh_s* parShapeMesh, GameObject* gameObjectMesh, const math::float4& color, GeometryType geometryType);
	void CreateMaterialComponent(GameObject* gameObjectMesh, const math::float4& color);
	void CreateTransformationComponent(GameObject* gameObject);
	void GenerateVAO(Mesh* mesh);
};

#endif __MODULEMODELLOADER_H__
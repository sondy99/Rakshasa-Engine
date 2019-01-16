#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"
#include "ModuleTextures.h"

enum class GeometryType;

struct Mesh
{
	char* name;

	unsigned vao = 0;
	unsigned vbo = 0;
	unsigned ibo = 0;

	float* normals = nullptr;
	float* colors = nullptr;
	float* uvs = nullptr;
	unsigned verticesNumber = 0;
	float* vertices = nullptr;
	unsigned indicesNumber = 0;
	unsigned* indices = nullptr;

	unsigned normalsOffset = 0;
	unsigned texturesOffset = 0;
	unsigned vertexSize = 0;
};

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh* mesh);
	~ComponentMesh();

	void DrawProperties() override;
	Component* Clone() override;

	void Save(const Config* config) override;
	void Load(const Config* config, rapidjson::Value& value) override;

	void CreateBoundingBox();
	void UpdateGlobalBoundingBox();
public:
	Mesh* mesh = nullptr;
	AABB localBoundingBox = AABB();
	AABB globalBoundingBox = AABB();
	bool isWireframeActive = false;
	GeometryType geometryType;
private:
	void LoadMesh(const char* name);
	void CleanMesh();
private:
	std::string labelCurrentFileMeshSelected;
};

#endif __COMPONENTMESH_H__
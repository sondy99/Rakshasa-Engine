#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"
#include "ModuleTextures.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh mesh);
	~ComponentMesh();

	void DrawProperties() override;
	Component* Clone() override;

	void Save(Config* config) override;
	void Load(Config* config, rapidjson::Value& value) override;

	void CreateBoundingBox();
	void UpdateGlobalBoundingBox();
public:
	Mesh mesh;
	AABB localBoundingBox = AABB();
	AABB globalBoundingBox = AABB();
	bool isWireframeActive = false;
private:
	void LoadMesh(const char* name);
private:
	std::string labelCurrentFileMeshSelected;
};

#endif __COMPONENTMESH_H__
#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"
#include "ModuleTextures.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh mesh);
	~ComponentMesh();

	void DrawProperties() override;
	Component* clone() override;
public:
	Mesh mesh;
	AABB boundingBox;
};

#endif __COMPONENTMESH_H__
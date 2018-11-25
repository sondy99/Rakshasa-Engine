#include "ComponentMesh.h"

ComponentMesh::ComponentMesh(GameObject* gameObject, ComponentType componentType, Mesh mesh) 
	: Component(gameObject, componentType), mesh(mesh)
{
}

ComponentMesh::~ComponentMesh()
{
}

#include "ComponentMesh.h"

ComponentMesh::ComponentMesh()
{
}

ComponentMesh::ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh mesh)
	: Component(gameObjectParent, componentType), mesh(mesh)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::DrawProperties()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Text("Triangles count: %d", mesh.verticesNumber / 3);
		ImGui::Text("Vertices count: %d", mesh.verticesNumber);
	}
}

Component * ComponentMesh::clone()
{
	ComponentMesh* result = new ComponentMesh();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	result->mesh = mesh;

	return result;
}

#include "ComponentMesh.h"

#include <assimp/scene.h>

#include "MathGeoLib.h"

ComponentMesh::ComponentMesh()
{
}

ComponentMesh::ComponentMesh(GameObject* gameObjectParent, ComponentType componentType)
	: Component(gameObjectParent, componentType)
{
}

ComponentMesh::ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh mesh)
	: Component(gameObjectParent, componentType), mesh(mesh)
{
	aiMesh* aiMesh = mesh.mesh;

	boundingBox.SetNegativeInfinity();
	boundingBox.Enclose((float3*)aiMesh->mVertices, mesh.verticesNumber);
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::DrawProperties()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (ImGui::SmallButton("Add"))
		{
		}
		ImGui::SameLine();
		DrawDeleteComponent();

		ImGui::Text("Triangles count: %d", mesh.verticesNumber / 3);
		ImGui::Text("Vertices count: %d", mesh.verticesNumber);

		ImGui::Checkbox("Wireframe active", &isWireframeActive);
	}
}

Component * ComponentMesh::Clone()
{
	ComponentMesh* result = new ComponentMesh();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	result->mesh = mesh;
	result->boundingBox = boundingBox;

	return result;
}

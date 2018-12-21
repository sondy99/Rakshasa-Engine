#include "ComponentMesh.h"

#include "Application.h"

#include "GameObject.h"

#include "ComponentTransformation.h"

#include "ModuleRender.h"

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

	localBoundingBox.SetNegativeInfinity();
	localBoundingBox.Enclose((float3*)aiMesh->mVertices, mesh.verticesNumber);
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
	ComponentMesh* result = App->renderer->CreateComponentMesh();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	result->mesh = mesh;
	result->localBoundingBox = localBoundingBox;

	return result;
}

void ComponentMesh::UpdateGlobalBoundingBox()
{
	ComponentTransformation* transformation = (ComponentTransformation*)gameObjectParent->GetComponent(ComponentType::TRANSFORMATION);

	if (transformation != nullptr)
	{
		globalBoundingBox = localBoundingBox;
		globalBoundingBox.TransformAsAABB(transformation->globalModelMatrix);
	}
}

#include "ComponentMesh.h"

#include "Application.h"

#include "GameObject.h"

#include "ComponentTransformation.h"

#include "ModuleRender.h"
#include "ModuleLibrary.h"
#include "ModuleModelLoader.h"

#include "ImporterMesh.h"

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
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::DrawProperties()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Button("Mesh options");

		if (ImGui::IsItemClicked(0))
		{
			ImGui::OpenPopup("MeshOptionsContextualMenu");
		}

		if (ImGui::BeginPopup("MeshOptionsContextualMenu"))
		{
			ImGui::PopID();
			ImGui::PushID("DeleteMesh");
			if (ImGui::Button("Delete mesh     "))
			{
			}
			ImGui::PopID();
			ImGui::PushID("DeleteMeshComponent");
			if (ImGui::Button("Delete component"))
			{
				isMarkToBeDeleted = true;
			}
			ImGui::PopID();
			ImGui::EndPopup();
		}

		ImGui::Separator();

		std::vector<std::string> fileMeshList = App->library->GetFileMeshList();
		labelCurrentFileMeshSelected;
		fileMeshList.insert(fileMeshList.begin(), "Select mesh");

		if (fileMeshList.size() > 0)
		{
			if (ImGui::BeginCombo("##meshCombo", labelCurrentFileMeshSelected.c_str()))
			{
				for (std::vector<std::string>::iterator iterator = fileMeshList.begin(); iterator != fileMeshList.end(); ++iterator)
				{
					bool isSelected = (labelCurrentFileMeshSelected == (*iterator));
					if (ImGui::Selectable((*iterator).c_str(), isSelected))
					{
						labelCurrentFileMeshSelected = (*iterator);

						App->modelLoader->CleanUpMesh(mesh);
						ImporterMesh::Load(&mesh, labelCurrentFileMeshSelected.c_str());
						App->modelLoader->GenerateMesh(mesh);
						CreateBoundingBox();
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Separator();

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

void ComponentMesh::CreateBoundingBox()
{
	if (mesh.vertices != nullptr)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose((float3*)mesh.vertices, mesh.verticesNumber);
	}
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

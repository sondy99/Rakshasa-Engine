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

#include "Config.h"

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
	if (mesh.indices != nullptr)
	{
		delete[]mesh.indices;
		mesh.indices = nullptr;
	}

	if (mesh.vertices != nullptr)
	{
		delete[]mesh.vertices;
		mesh.vertices = nullptr;
	}

	if (mesh.uvs != nullptr)
	{
		delete[]mesh.uvs;
		mesh.uvs = nullptr;
	}

	if (mesh.normals != nullptr)
	{
		delete[]mesh.normals;
		mesh.normals = nullptr;
	}

	if (mesh.colors != nullptr)
	{
		delete[]mesh.colors;
		mesh.colors = nullptr;
	}
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

		std::vector<std::string> fileMeshesList = App->library->GetFileMeshList();
		labelCurrentFileMeshSelected;
		fileMeshesList.insert(fileMeshesList.begin(), "Select mesh");

		if (fileMeshesList.size() > 0)
		{
			if (ImGui::BeginCombo("##meshCombo", labelCurrentFileMeshSelected.c_str()))
			{
				for (std::vector<std::string>::iterator iterator = fileMeshesList.begin(); iterator != fileMeshesList.end(); ++iterator)
				{
					bool isSelected = (labelCurrentFileMeshSelected == (*iterator));
					if (ImGui::Selectable((*iterator).c_str(), isSelected))
					{
						labelCurrentFileMeshSelected = (*iterator);

						LoadMesh(labelCurrentFileMeshSelected.c_str());
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

void ComponentMesh::Save(Config * config)
{
	config->StartObject();

	config->AddComponentType("componentType", componentType);
	config->AddString("gameObjectParent", gameObjectParent->uuid);
	config->AddString("labelCurrentFileMeshSelected", labelCurrentFileMeshSelected.c_str());
	config->AddBool("isWireframeActive", isWireframeActive);

	config->EndObject();
}

void ComponentMesh::Load(Config* config, rapidjson::Value& value)
{
	labelCurrentFileMeshSelected = config->GetString("labelCurrentFileMeshSelected", value);
	isWireframeActive = config->GetBool("isWireframeActive", value);

	LoadMesh(labelCurrentFileMeshSelected.c_str());
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

void ComponentMesh::LoadMesh(const char * name)
{
	App->modelLoader->CleanUpMesh(mesh);
	ImporterMesh::Load(&mesh, name);
	App->modelLoader->GenerateMesh(mesh);
	CreateBoundingBox();
}

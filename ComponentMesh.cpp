#include "ComponentMesh.h"

#include "Application.h"

#include "GameObject.h"

#include "ComponentTransformation.h"

#include "ModuleRender.h"
#include "ModuleLibrary.h"
#include "ModuleModelLoader.h"
#include "ModuleScene.h"

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

ComponentMesh::ComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh* mesh)
	: Component(gameObjectParent, componentType), mesh(mesh)
{
}

ComponentMesh::~ComponentMesh()
{
	CleanMesh();
}

void ComponentMesh::DrawProperties()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (ImGui::Button("Delete component"))
		{
			isMarkToBeDeleted = true;
		}

		ImGui::Separator();

		labelCurrentFileMeshSelected;
		if (ImGui::BeginCombo("##meshCombo", labelCurrentFileMeshSelected.c_str()))
		{
			for (std::vector<std::string>::iterator iterator = App->library->GetFileMeshList()->begin(); iterator != App->library->GetFileMeshList()->end(); ++iterator)
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

		ImGui::Separator();

		if (mesh != nullptr)
		{
			ImGui::Text("Triangles count: %d", mesh->verticesNumber / 3);
			ImGui::Text("Vertices count: %d", mesh->verticesNumber);
		}

		ImGui::Checkbox("Wireframe active", &isWireframeActive);
	}
}

Component * ComponentMesh::Clone()
{
	ComponentMesh* result = App->renderer->CreateComponentMesh();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	ImporterMesh::Load(result->mesh, (char*)mesh->name);
	result->labelCurrentFileMeshSelected = labelCurrentFileMeshSelected;
	result->localBoundingBox = localBoundingBox;
	result->geometryType = geometryType;
	result->isWireframeActive = isWireframeActive;

	return result;
}

void ComponentMesh::Save(const Config * config)
{
	config->StartObject();

	config->AddComponentType("componentType", componentType);
	config->AddString("gameObjectParent", gameObjectParent->uuid);

	if (!labelCurrentFileMeshSelected.empty())
	{
		config->AddString("labelCurrentFileMeshSelected", labelCurrentFileMeshSelected.c_str());
	}
	else
	{
		config->AddGeometryType("geometryType", geometryType);
	}

	config->AddBool("isWireframeActive", isWireframeActive);

	config->EndObject();
}

void ComponentMesh::Load(const Config* config, rapidjson::Value& value)
{
	isWireframeActive = config->GetBool("isWireframeActive", value);

	if (value.HasMember("labelCurrentFileMeshSelected"))
	{
		labelCurrentFileMeshSelected = config->GetString("labelCurrentFileMeshSelected", value);
		LoadMesh(labelCurrentFileMeshSelected.c_str());
	}
	else
	{
		geometryType = config->GetGeometryType("geometryType", value);

		par_shapes_mesh_s* parMesh = App->modelLoader->CreateParShapesByGeometryType(geometryType);
		mesh = new Mesh();
		App->modelLoader->GenerateMesh(parMesh, mesh);
		App->modelLoader->FreeParShapes(parMesh);
	}
}

void ComponentMesh::CreateBoundingBox()
{
	if (mesh->vertices != nullptr)
	{
		localBoundingBox.SetNegativeInfinity();
		localBoundingBox.Enclose((float3*)mesh->vertices, mesh->verticesNumber);
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

void ComponentMesh::LoadMesh(const char* name)
{
	CleanMesh();
	ImporterMesh::Load(mesh, (char*)name);
	App->modelLoader->GenerateMesh(mesh);
	CreateBoundingBox();
}

void ComponentMesh::CleanMesh()
{
	if (mesh != nullptr)
	{
		App->modelLoader->CleanUpMesh(mesh);
		ImporterMesh::RemoveMesh(mesh->name);
		mesh = nullptr;
	}
}


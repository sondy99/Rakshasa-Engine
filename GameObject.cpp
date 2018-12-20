#include "GameObject.h"
#include "crossguid/guid.hpp"

#include "Application.h"

#include "ModuleRender.h"

#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	xg::Guid guid = xg::newGuid();

	std::string uuidAux = guid.str();
	sprintf_s(uuid, uuidAux.c_str());
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

void GameObject::DrawProperties()
{
	if (isSelected)
	{
		if (ImGui::CollapsingHeader("Game object properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Delete"))
			{
				RemoveGameObject(this);
			}
			ImGui::SameLine();
			ImGui::Button("New Component");

			if (ImGui::IsItemClicked(0))
			{
				ImGui::OpenPopup("NewComponentContextualMenu");
			}

			if (ImGui::BeginPopup("NewComponentContextualMenu"))
			{
				if (ImGui::Button("Mesh    "))
				{
					CreateComponent(ComponentType::MESH);
				}
				if (ImGui::Button("Material"))
				{
					CreateComponent(ComponentType::MATERIAL);
				}
				if (ImGui::Button("Camera  "))
				{
					CreateComponent(ComponentType::CAMERA);
				}
				ImGui::EndPopup();
			}

			ImGui::Separator();
			ImGui::Text("Game name:");
			ImGui::SameLine();
			ImGui::InputText("##", &name[0], 40);
			ImGui::Text("Model selected has %d childs.", childrens.size());
			ImGui::Checkbox("Active", &active);
		}

		if (components.size() > 0)
		{
			for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end();)
			{
				if (!(*iterator)->isMarkToBeDeleted)
				{
					(*iterator)->DrawProperties(); 
					iterator++;
				}
				else
				{
					iterator = RemoveComponent(iterator);
				}

			}
		}
	}
}

void GameObject::RemoveGameObject(GameObject* mainObjectToDelete)
{
	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end();)
	{
		iterator = RemoveComponent(iterator);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end();)
	{
		//TODO: change this to not use recursivity
		(*iterator)->RemoveGameObject(nullptr);
		RELEASE(*iterator);
		iterator = childrens.erase(iterator);
	}

	if (mainObjectToDelete != nullptr && mainObjectToDelete == this)
	{
		parent->childrens.remove(this);
	}
}

void GameObject::DuplicateGameObject(GameObject* newGameObjectParent)
{
	GameObject* newGameObject = Clone();

	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		Component* clonedComponenet = (*iterator)->Clone();
		clonedComponenet->gameObjectParent = newGameObject;

		newGameObject->components.push_back(clonedComponenet);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end(); iterator++)
	{
		//TODO: change this to not use recursivity
		(*iterator)->DuplicateGameObject(newGameObject);
	}

	if (newGameObjectParent == nullptr)
	{
		parent->childrens.push_back(newGameObject);
	}
	else
	{
		newGameObject->parent = newGameObjectParent;
		newGameObjectParent->childrens.push_back(newGameObject);
	}

}

GameObject* GameObject::Clone()
{
	GameObject* result = new GameObject(name.c_str(), parent);

	result->active = active;
	result->globalBoundingBox = globalBoundingBox;
	result->localBoundingBox = localBoundingBox;

	return result;
}

Component* GameObject::GetComponent(ComponentType componentType)
{
	Component* result = nullptr;

	for (auto &component : components)
	{
		if (component->componentType == componentType)
		{
			result = component;
			break;
		}
	}

	return result;
}



void GameObject::UpdateBoundingBoxForGameObjectWithMesh()
{
	std::vector<float3> minMaxPointsAABB;

	ComponentMesh* componentMesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (componentMesh != nullptr)
	{
		minMaxPointsAABB.push_back(componentMesh->boundingBox.minPoint);
		minMaxPointsAABB.push_back(componentMesh->boundingBox.maxPoint);
	
		if (minMaxPointsAABB.size() > 0)
		{
			localBoundingBox = localBoundingBox.MinimalEnclosingAABB(&minMaxPointsAABB[0], minMaxPointsAABB.size());

			ComponentTransformation* transformation = (ComponentTransformation*)GetComponent(ComponentType::TRANSFORMATION);

			if (transformation != nullptr)
			{
				globalBoundingBox = localBoundingBox;
				globalBoundingBox.TransformAsAABB(transformation->globalModelMatrix);
			}
		}
	}
}

void GameObject::UpdateBoundingBoxForGameObjectWithOutMesh()
{
	for (auto &gameObjectChild : childrens)
	{
		//TODO: change this to not use recursivity
		gameObjectChild->UpdateBoundingBoxForGameObjectWithOutMesh();
	}

	ComponentMesh* mesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (mesh == nullptr)
	{
		std::vector<float3> minMaxPointsAABB;

		for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end(); iterator++)
		{
			minMaxPointsAABB.push_back((*iterator)->globalBoundingBox.minPoint);
			minMaxPointsAABB.push_back((*iterator)->globalBoundingBox.maxPoint);
		}

		if (minMaxPointsAABB.size() > 0)
		{
			localBoundingBox = localBoundingBox.MinimalEnclosingAABB(&minMaxPointsAABB[0], minMaxPointsAABB.size());

			ComponentTransformation* transformation = (ComponentTransformation*)GetComponent(ComponentType::TRANSFORMATION);

			if (transformation != nullptr)
			{
				globalBoundingBox = localBoundingBox;
				globalBoundingBox.TransformAsAABB(transformation->globalModelMatrix);
			}
		}
	}
}

void GameObject::CreateComponent(ComponentType componentType)
{
	switch (componentType)
	{
	case ComponentType::MESH:
	{
		Component * mesh = GetComponent(ComponentType::MESH);
		if (mesh == nullptr)
		{
			components.push_back((App->renderer->CreateComponentMesh(this, ComponentType::MESH)));
		}
		else
		{
			LOG("Is not possible to add more than one mesh to an specific gameobject");
		}
	}
	break;
	case ComponentType::MATERIAL:
	{
		components.push_back((new ComponentMaterial(this, ComponentType::MATERIAL)));
	}
	break;
	case ComponentType::CAMERA:
	{
		components.push_back((new ComponentCamera(this, ComponentType::CAMERA)));
	}
	break;
	}
}

std::list<Component*>::iterator GameObject::RemoveComponent(std::list<Component*>::iterator iteratorComponentToBeRemove)
{
	App->renderer->RemoveMesh(*iteratorComponentToBeRemove);
	RELEASE(*iteratorComponentToBeRemove);
	std::list<Component*>::iterator iterator = components.erase(iteratorComponentToBeRemove);
	
	return iterator;
}

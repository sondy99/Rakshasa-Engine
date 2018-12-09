#include "GameObject.h"
#include "crossguid/guid.hpp"

#include "ComponentMesh.h"
#include "ComponentTransformation.h"

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

void GameObject::Remove(GameObject* mainObjectToDelete)
{
	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end();)
	{
		RELEASE(*iterator);
		iterator = components.erase(iterator);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end();)
	{
		//TODO: change this to not use recursivity
		(*iterator)->Remove(nullptr);
		RELEASE(*iterator);
		iterator = childrens.erase(iterator);
	}

	if (mainObjectToDelete != nullptr && mainObjectToDelete == this)
	{
		parent->childrens.Remove(this);
	}
}

void GameObject::Duplicate(GameObject* newGameObjectParent)
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
		(*iterator)->Duplicate(newGameObject);
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
	result->boundingBox = boundingBox;
	result->originalboundingBox = originalboundingBox;

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

void GameObject::UpdateBoundingBox()
{
	std::vector<float3> minMaxPointsAABB;

	ComponentMesh* componentMesh = (ComponentMesh*)GetComponent(ComponentType::MESH);

	if (componentMesh != nullptr)
	{
		minMaxPointsAABB.push_back(componentMesh->boundingBox.minPoint);
		minMaxPointsAABB.push_back(componentMesh->boundingBox.maxPoint);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end(); iterator++)
	{
		(*iterator)->UpdateBoundingBoxTransformation();

		minMaxPointsAABB.push_back((*iterator)->boundingBox.minPoint);
		minMaxPointsAABB.push_back((*iterator)->boundingBox.maxPoint);
	}

	if (minMaxPointsAABB.size() > 0)
	{
		originalboundingBox = boundingBox.MinimalEnclosingAABB(&minMaxPointsAABB[0], minMaxPointsAABB.size());
		boundingBox = originalboundingBox;
	}
}

void GameObject::UpdateBoundingBoxTransformation()
{
	ComponentTransformation* transformation = (ComponentTransformation*)GetComponent(ComponentType::TRANSFORMATION);

	if (transformation != nullptr)
	{
		boundingBox = originalboundingBox;
		boundingBox.TransformAsAABB(transformation->globalModelMatrix);
	}

	for (auto &gameObjectChild : childrens)
	{
		ComponentTransformation* transformation = (ComponentTransformation*)gameObjectChild->GetComponent(ComponentType::TRANSFORMATION);

		//TODO: change this to not use recursivity
		gameObjectChild->UpdateBoundingBoxTransformation();
	}
}

#include "GameObject.h"
#include "crossguid/guid.hpp"

#include "Application.h"

#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

#include "Config.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	xg::Guid guid = xg::newGuid();

	std::string uuidAux = guid.str();
	sprintf_s(uuid, uuidAux.c_str());
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end();)
	{
		if ((*iterator)->componentType == ComponentType::MATERIAL)
		{
			App->textures->CleanUpFromList((ComponentMaterial*)*iterator);
		}
		if ((*iterator)->componentType == ComponentType::MESH)
		{
			App->renderer->CleanUpFromList((ComponentMesh*)*iterator);
		}
		if ((*iterator)->componentType == ComponentType::CAMERA)
		{
			App->camera->CleanUpFromList((ComponentCamera*)*iterator);
		}

		iterator = components.erase(iterator);
	}

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end();)
	{
		RELEASE(*iterator);
		iterator = childrens.erase(iterator);
	}

	parent = nullptr;
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
			ImGui::Button("Game object options");

			if (ImGui::IsItemClicked(0))
			{
				ImGui::OpenPopup("GameObjectOptionsContextualMenu");
			}

			if (ImGui::BeginPopup("GameObjectOptionsContextualMenu"))
			{
				if (ImGui::Button("Delete  "))
				{
					RemoveGameObject(this);
				}
				ImGui::Separator();
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
			
			bool haveComponentRecursive = HaveComponentRecursive(ComponentType::MESH);

			if (haveComponentRecursive)
			{
				ImGui::Checkbox("Active", &active);

				bool previousGameObjectStatic = gameObjectStatic;
				ImGui::Checkbox("Static", &gameObjectStatic);

				if (previousGameObjectStatic != gameObjectStatic)
				{
					if (!gameObjectStatic)
					{
						App->scene->quadTree.RemoveGameObject(this);
					}
					else
					{
						std::list<Component*> componentList; 
						GetComponentRecursive(componentList, ComponentType::MESH);

						if (componentList.size() > 0)
						{
							for (std::list<Component*>::iterator iterator = componentList.begin(); iterator != componentList.end(); ++iterator)
							{
								((ComponentMesh*)(*iterator))->UpdateGlobalBoundingBox();
								App->scene->quadTree.InsertGameObject(((ComponentMesh*)(*iterator))->gameObjectParent, true);
							}
						}
					}
				}
			}
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
	result->gameObjectStatic = gameObjectStatic;

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

void GameObject::GetComponentRecursive(std::list<Component*>& componentList, ComponentType componentType)
{
	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end(); iterator++)
	{
		//TODO: change this to not use recursivity
		(*iterator)->GetComponentRecursive(componentList, componentType);
	}

	Component* component = GetComponent(componentType);

	if (component != nullptr)
	{
		componentList.push_back(component);
	}
}

bool GameObject::HaveComponentRecursive(ComponentType componentType)
{
	bool result = false;

	for (std::list<GameObject*>::iterator iterator = childrens.begin(); iterator != childrens.end(); iterator++)
	{
		//TODO: change this to not use recursivity
		result = (*iterator)->HaveComponentRecursive(componentType);

		if (result)
		{
			break;
		}
	}

	if (!result)
	{
		Component* component = GetComponent(componentType);

		if (component != nullptr)
		{
			result = true;
		}
	}

	return result;
}

Component* GameObject::CreateComponent(ComponentType componentType)
{
	Component* result = nullptr;

	switch (componentType)
	{
	case ComponentType::MESH:
	{
		Component * mesh = GetComponent(ComponentType::MESH);
		if (mesh == nullptr)
		{
			result = App->renderer->CreateComponentMesh(this, ComponentType::MESH);
			components.push_back(result);
		}
		else
		{
			LOG("Is not possible to add more than one mesh to an specific gameobject");
		}
	}
	break;
	case ComponentType::MATERIAL:
	{
		Component * material = GetComponent(ComponentType::MATERIAL);

		if (material == nullptr)
		{
			result = App->textures->CreateComponentMaterial(this, ComponentType::MATERIAL);
			components.push_back(result);
		}
		else
		{
			LOG("Is not possible to add more than one material to an specific gameobject");
		}
	}
	break;
	case ComponentType::CAMERA:
	{
		result = App->camera->CreateComponentCamera(this, ComponentType::CAMERA);
		components.push_back(result);
	}
	break;
	case ComponentType::TRANSFORMATION:
	{
		Component* transformation = GetComponent(ComponentType::TRANSFORMATION);

		if (transformation == nullptr)
		{
			float3 position = { 0.0f,0.0f,0.0f };
			float3 scale = { 1.0f,1.0f,1.0f };
			Quat quatRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);

			result = new ComponentTransformation(this, ComponentType::TRANSFORMATION, position, scale, quatRotation);
			components.push_back(result);
		}
		else
		{
			LOG("Is not possible to add more than one transformation to an specific gameobject");
		}
	}
	}

	return result;
}

bool GameObject::Save(Config* config)
{
	config->StartObject();

	config->AddString("uuid", uuid);
	config->AddString("name", name.c_str());

	if (parent != nullptr)
	{
		config->AddString("parent", parent->uuid);
	}

	config->AddBool("active", active);
	config->AddBool("gameObjectStatic", gameObjectStatic);
	
	config->StartArray("components");

	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		(*iterator)->Save(config);
	}

	config->EndArray();

	config->EndObject();

	return true;
}

void GameObject::Load(Config* config, rapidjson::Value & value)
{
	const char* newUuid = config->GetString("uuid", value);
	sprintf_s(uuid, newUuid);

	active = config->GetBool("active", value);
	gameObjectStatic = config->GetBool("gameObjectStatic", value);
	
	rapidjson::Value components = value["components"].GetArray();
	for (rapidjson::Value::ValueIterator it = components.Begin(); it != components.End(); it++)
	{
		Component* component = CreateComponent(config->GetComponentType("componentType", (*it)));

		if (component != nullptr)
		{
			component->Load(config, (*it));
		}
	}
}

std::list<Component*>::iterator GameObject::RemoveComponent(std::list<Component*>::iterator iteratorComponentToBeRemove)
{
	App->renderer->RemoveMeshComponent(*iteratorComponentToBeRemove);
	App->textures->RemoveMaterialComponent(*iteratorComponentToBeRemove);
	App->camera->RemoveCameraComponent(*iteratorComponentToBeRemove);
	RELEASE(*iteratorComponentToBeRemove);
	std::list<Component*>::iterator iterator = components.erase(iteratorComponentToBeRemove);
	
	return iterator;
}

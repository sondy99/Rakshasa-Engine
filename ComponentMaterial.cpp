#include "ComponentMaterial.h"

#include "Application.h"

#include "ModuleTextures.h"

ComponentMaterial::ComponentMaterial()
{
}

ComponentMaterial::ComponentMaterial(GameObject* gameObjectParent, ComponentType componentType)
	: Component(gameObjectParent, componentType)
{
}

ComponentMaterial::ComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material)
	: Component(gameObjectParent, componentType), material(material)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::DrawProperties()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::Button("Material options");

		if (ImGui::IsItemClicked(0))
		{
			ImGui::OpenPopup("MaterialOptionsContextualMenu");
		}

		if (ImGui::BeginPopup("MaterialOptionsContextualMenu"))
		{
			ImGui::PushID("AddMaterial");
			if (ImGui::Button("Add from library"))
			{
			}
			ImGui::PopID();
			ImGui::PushID("DeleteMaterial");
			if (ImGui::Button("Delete material"))
			{
				App->textures->RemoveMaterial(this);
			}
			ImGui::PopID();
			ImGui::PushID("DeleteMaterialComponent");
			if (ImGui::Button("Delete component"))
			{
				isMarkToBeDeleted = true;
			}
			ImGui::PopID();
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ImGui::ColorEdit3("Default color", (float*)&material.color);
		ImGui::Text("Dimensions: %dx%d", material.width, material.height);
		ImGui::Image((ImTextureID)material.texture0, ImVec2(200, 200));
	}
}

Component * ComponentMaterial::Clone()
{
	ComponentMaterial* result = App->textures->CreateComponentMaterial();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	result->material = material;

	return result;
}

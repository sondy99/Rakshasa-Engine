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
			if (ImGui::Button("Add from library    "))
			{
			}
			ImGui::PopID();
			ImGui::PushID("DeleteMaterial");
			if (ImGui::Button("Delete all materials"))
			{
				App->textures->RemoveMaterial(this);
			}
			ImGui::PopID();
			ImGui::PushID("DeleteMaterialComponent");
			if (ImGui::Button("Delete component   "))
			{
				isMarkToBeDeleted = true;
			}
			ImGui::PopID();
			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Diffuse"))
		{
			ImGui::ColorEdit3("Diffuse color", (float*)&material.diffuseColor);
			ImGui::PushID("diffuseCheck");
			ImGui::Checkbox("", &diffuseCheck);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputText("Diffuse path", &diffusePath[0], 150, ImGuiInputTextFlags_ReadOnly);
			if (diffuseCheck)
			{
				occlusionCheck = specularCheck = emissiveCheck = false;
				materialTypeSelected = MaterialTypeSelected::DIFFUSE_MAP;
			}
			ImGui::Text("Dimensions: %dx%d", material.diffuseWidth, material.diffuseHeight);
			ImGui::Image((ImTextureID)material.diffuseMap, ImVec2(200, 200));
			ImGui::SliderFloat("K diffuse", &material.diffuseK, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Ambient"))
		{
			ImGui::PushID("occlusionCheck");
			ImGui::Checkbox("", &occlusionCheck);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputText("Ambient path", &occlusionPath[0], 150, ImGuiInputTextFlags_ReadOnly);
			if (occlusionCheck)
			{
				diffuseCheck = specularCheck = emissiveCheck = false;
				materialTypeSelected = MaterialTypeSelected::OCCLUSION_MAP;
			}
			ImGui::Text("Dimensions: %dx%d", material.ambientWidth, material.ambientHeight);
			ImGui::Image((ImTextureID)material.occlusionMap, ImVec2(200, 200));
			ImGui::SliderFloat("K ambient", &material.ambientK, 0.0f, 1.0f);
		}
		
		if (ImGui::CollapsingHeader("Specular"))
		{
			ImGui::ColorEdit3("Specular color", (float*)&material.specularColor);
			ImGui::PushID("specularCheck");
			ImGui::Checkbox("", &specularCheck);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputText("Specular path", &specularPath[0], 150, ImGuiInputTextFlags_ReadOnly);
			if (specularCheck)
			{
				occlusionCheck = diffuseCheck = emissiveCheck = false;
				materialTypeSelected = MaterialTypeSelected::SPECULAR_MAP;
			}
			ImGui::Text("Dimensions: %dx%d", material.specularWidth, material.specularHeight);
			ImGui::Image((ImTextureID)material.specularMap, ImVec2(200, 200));
			ImGui::SliderFloat("K specular", &material.specularK, 0.0f, 1.0f);
			ImGui::SliderFloat("K shininess", &material.shininess, 0.0f, 128.0f);
		}

		if (ImGui::CollapsingHeader("Emissive"))
		{
			ImGui::ColorEdit3("Emissive color", (float*)&material.emissiveColor);
			ImGui::PushID("emissiveCheck");
			ImGui::Checkbox("", &emissiveCheck);
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::InputText("Emissive path", &emissivePath[0], 150, ImGuiInputTextFlags_ReadOnly);
			if (emissiveCheck)
			{
				occlusionCheck = diffuseCheck = specularCheck = false;
				materialTypeSelected = MaterialTypeSelected::EMISSIVE_MAP;
			}
			ImGui::Text("Dimensions: %dx%d", material.emissiveWidth, material.emissiveHeight);
			ImGui::Image((ImTextureID)material.emissiveMap, ImVec2(200, 200));
		}

		if (!occlusionCheck && !diffuseCheck && !specularCheck && !emissiveCheck)
		{
			materialTypeSelected = MaterialTypeSelected::NO_TYPE_SELECTED;
		}
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

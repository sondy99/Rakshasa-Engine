#include "ComponentMaterial.h"

#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleLibrary.h"

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
			DrawComboBoxMaterials("DiffuseComboTextures", MaterialTypeSelected::DIFFUSE_MAP, labelDiffuseCurrentFileTextureSelected);
			ImGui::Text("Dimensions: %dx%d", material.diffuseWidth, material.diffuseHeight);
			ImGui::Image((ImTextureID)material.diffuseMap, ImVec2(200, 200));
			ImGui::SliderFloat("K diffuse", &material.diffuseK, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Ambient"))
		{
			DrawComboBoxMaterials("OcclusionComboTextures", MaterialTypeSelected::OCCLUSION_MAP, labelOcclusionCurrentFileTextureSelected);
			ImGui::Text("Dimensions: %dx%d", material.ambientWidth, material.ambientHeight);
			ImGui::Image((ImTextureID)material.occlusionMap, ImVec2(200, 200));
			ImGui::SliderFloat("K ambient", &material.ambientK, 0.0f, 1.0f);
		}
		
		if (ImGui::CollapsingHeader("Specular"))
		{
			ImGui::ColorEdit3("Specular color", (float*)&material.specularColor);
			DrawComboBoxMaterials("SpecularComboTextures", MaterialTypeSelected::SPECULAR_MAP, labelSpecularCurrentFileTextureSelected);
			ImGui::Text("Dimensions: %dx%d", material.specularWidth, material.specularHeight);
			ImGui::Image((ImTextureID)material.specularMap, ImVec2(200, 200));
			ImGui::SliderFloat("K specular", &material.specularK, 0.0f, 1.0f);
			ImGui::SliderFloat("K shininess", &material.shininess, 0.0f, 128.0f);
		}

		if (ImGui::CollapsingHeader("Emissive"))
		{
			ImGui::ColorEdit3("Emissive color", (float*)&material.emissiveColor);
			DrawComboBoxMaterials("EmissiveComboTextures", MaterialTypeSelected::EMISSIVE_MAP, labelEmissiveCurrentFileTextureSelected);
			ImGui::Text("Dimensions: %dx%d", material.emissiveWidth, material.emissiveHeight);
			ImGui::Image((ImTextureID)material.emissiveMap, ImVec2(200, 200));
		}
	}
}

void ComponentMaterial::DrawComboBoxMaterials(const char * id, MaterialTypeSelected materialTypeSelected, static std::string &labelCurrentFileTextureSelected)
{
	std::vector<std::string> fileTexturesList = App->library->GetFileTexturesList();
	fileTexturesList.insert(fileTexturesList.begin(), "Select a Texture");

	if (fileTexturesList.size() > 0)
	{
		ImGui::PushID(id);
		if (ImGui::BeginCombo("##", labelCurrentFileTextureSelected.c_str()))
		{
			for (std::vector<std::string>::iterator iterator = fileTexturesList.begin(); iterator != fileTexturesList.end(); ++iterator)
			{
				bool isSelected = (labelCurrentFileTextureSelected == (*iterator).c_str());
				if (ImGui::Selectable((*iterator).c_str(), isSelected))
				{
					labelCurrentFileTextureSelected = (*iterator).c_str();
					App->textures->LoadMaterial(labelCurrentFileTextureSelected.c_str(), this, materialTypeSelected);
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
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


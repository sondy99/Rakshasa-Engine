#include "ComponentMaterial.h"

#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleLibrary.h"

#include "GameObject.h"

#include "Config.h"

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
			ImGui::PushID("DeleteMaterial");
			if (ImGui::Button("Delete all materials"))
			{
				App->textures->RemoveMaterial(this);
				labelDiffuseCurrentFileTextureSelected = "Select a Texture";
				labelEmissiveCurrentFileTextureSelected = "Select a Texture";
				labelOcclusionCurrentFileTextureSelected = "Select a Texture";
				labelSpecularCurrentFileTextureSelected= "Select a Texture";
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
	ImGui::PushID(id);
	if (ImGui::BeginCombo("##", labelCurrentFileTextureSelected.c_str()))
	{
		for (std::vector<std::string>::iterator iterator = App->library->GetFileTexturesList()->begin(); iterator != App->library->GetFileTexturesList()->end(); ++iterator)
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

void ComponentMaterial::LoadMaterial(const char * name)
{
}

Component* ComponentMaterial::Clone()
{
	ComponentMaterial* result = App->textures->CreateComponentMaterial();

	result->gameObjectParent = gameObjectParent;
	result->componentType = componentType;
	result->material.ambientK = material.ambientK;
	result->material.ambientWidth = material.ambientWidth;
	result->material.ambientHeight = material.ambientHeight;

	result->material.diffuseColor = material.diffuseColor;
	result->material.diffuseK = material.diffuseK;
	result->material.diffuseWidth = material.diffuseWidth;
	result->material.diffuseHeight = material.diffuseHeight;

	result->material.specularColor = material.specularColor;
	result->material.specularK = material.specularK;
	result->material.shininess = material.shininess;
	result->material.specularWidth = material.specularWidth;
	result->material.specularHeight = material.specularHeight;

	result->material.emissiveColor = material.emissiveColor;
	result->material.emissiveWidth = material.emissiveWidth;
	result->material.emissiveHeight = material.emissiveHeight;

	result->labelDiffuseCurrentFileTextureSelected = labelDiffuseCurrentFileTextureSelected;
	App->textures->LoadMaterial(labelDiffuseCurrentFileTextureSelected.c_str(), result, MaterialTypeSelected::DIFFUSE_MAP);

	result->labelOcclusionCurrentFileTextureSelected = labelOcclusionCurrentFileTextureSelected;
	App->textures->LoadMaterial(labelOcclusionCurrentFileTextureSelected.c_str(), result, MaterialTypeSelected::OCCLUSION_MAP);

	result->labelSpecularCurrentFileTextureSelected = labelSpecularCurrentFileTextureSelected;
	App->textures->LoadMaterial(labelSpecularCurrentFileTextureSelected.c_str(), result, MaterialTypeSelected::SPECULAR_MAP);

	result->labelEmissiveCurrentFileTextureSelected = labelEmissiveCurrentFileTextureSelected;
	App->textures->LoadMaterial(labelEmissiveCurrentFileTextureSelected.c_str(), result, MaterialTypeSelected::EMISSIVE_MAP);

	return result;
}

void ComponentMaterial::Save(Config * config)
{
	config->StartObject();

	config->AddComponentType("componentType", componentType);
	config->AddString("gameObjectParent", gameObjectParent->uuid);

	config->AddString("labelDiffuseCurrentFileTextureSelected", labelDiffuseCurrentFileTextureSelected.c_str());
	config->AddFloat4("diffuseColor", material.diffuseColor);
	config->AddFloat("diffuseK", material.diffuseK);

	config->AddString("labelOcclusionCurrentFileTextureSelected", labelOcclusionCurrentFileTextureSelected.c_str());
	config->AddFloat("ambientK", material.ambientK);

	config->AddString("labelSpecularCurrentFileTextureSelected", labelSpecularCurrentFileTextureSelected.c_str());
	config->AddFloat4("specularColor", material.specularColor);
	config->AddFloat("specularK", material.specularK);
	config->AddFloat("shininess", material.shininess);

	config->AddString("labelEmissiveCurrentFileTextureSelected", labelEmissiveCurrentFileTextureSelected.c_str());
	config->AddFloat4("emissiveColor", material.emissiveColor);
	
	config->EndObject();
}

void ComponentMaterial::Load(Config* config, rapidjson::Value& value)
{
	labelDiffuseCurrentFileTextureSelected = config->GetString("labelDiffuseCurrentFileTextureSelected", value);
	material.diffuseColor = config->GetFloat4("diffuseColor", value);
	material.diffuseK = config->GetFloat("diffuseK", value);
	App->textures->LoadMaterial(labelDiffuseCurrentFileTextureSelected.c_str(), this, MaterialTypeSelected::DIFFUSE_MAP);

	labelOcclusionCurrentFileTextureSelected = config->GetString("labelOcclusionCurrentFileTextureSelected", value);
	material.ambientK = config->GetFloat("ambientK", value);
	App->textures->LoadMaterial(labelOcclusionCurrentFileTextureSelected.c_str(), this, MaterialTypeSelected::OCCLUSION_MAP);

	labelSpecularCurrentFileTextureSelected = config->GetString("labelSpecularCurrentFileTextureSelected", value);
	material.specularColor = config->GetFloat4("specularColor", value);
	material.specularK = config->GetFloat("specularK", value);
	material.shininess = config->GetFloat("shininess", value);
	App->textures->LoadMaterial(labelSpecularCurrentFileTextureSelected.c_str(), this, MaterialTypeSelected::SPECULAR_MAP);

	labelEmissiveCurrentFileTextureSelected = config->GetString("labelEmissiveCurrentFileTextureSelected", value);
	material.emissiveColor = config->GetFloat4("emissiveColor", value);
	App->textures->LoadMaterial(labelEmissiveCurrentFileTextureSelected.c_str(), this, MaterialTypeSelected::EMISSIVE_MAP);
}


#ifndef __COMPONENTMATERIAL_H_
#define __COMPONENTMATERIAL_H_

#include "Component.h"
#include "ModuleTextures.h"

#include "vector"
#include "string"

enum class MaterialTypeSelected
{
	NO_TYPE_SELECTED = 0,
	OCCLUSION_MAP,
	DIFFUSE_MAP,
	SPECULAR_MAP,
	EMISSIVE_MAP
};

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material);
	~ComponentMaterial();

	void DrawProperties() override;
	Component* Clone() override;

	void Save(Config* config) override;
	void Load(Config* config, rapidjson::Value& value) override;
public:
	Material material;
private:
	void DrawComboBoxMaterials(const char* id, MaterialTypeSelected materialTypeSelected, static std::string &labelCurrentFileTextureSelected);
	void LoadMaterial(const char* name);
private:
	std::string labelDiffuseCurrentFileTextureSelected;
	std::string labelOcclusionCurrentFileTextureSelected;
	std::string labelSpecularCurrentFileTextureSelected;
	std::string labelEmissiveCurrentFileTextureSelected;
};

#endif __COMPONENTMATERIAL_H_

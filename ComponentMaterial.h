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

struct Material
{
	unsigned occlusionMap = 0u; //ambient
	float ambientK = 0.5f;
	int ambientWidth = 0;
	int ambientHeight = 0;

	unsigned diffuseMap = 0u;
	math::float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float diffuseK = 1.0f;
	int diffuseWidth = 0;
	int diffuseHeight = 0;

	unsigned specularMap = 0u;
	math::float4 specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float specularK = 0.6f;
	float shininess = 64.0f;
	int specularWidth = 0;
	int specularHeight = 0;

	unsigned emissiveMap = 0u;
	math::float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int emissiveWidth = 0;
	int emissiveHeight = 0;
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

	void Save(const Config* config) override;
	void Load(const Config* config, rapidjson::Value& value) override;
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

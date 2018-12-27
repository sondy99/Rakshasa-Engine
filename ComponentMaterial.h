#ifndef __COMPONENTMATERIAL_H_
#define __COMPONENTMATERIAL_H_

#include "Component.h"
#include "ModuleTextures.h"

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
public:
	Material material;
	bool isSomeMaterialTypeSelected = false;
	MaterialTypeSelected materialTypeSelected = MaterialTypeSelected::NO_TYPE_SELECTED;

	std::string occlusionPath = "";
	std::string diffusePath = "";
	std::string specularPath = "";
	std::string emissivePath = "";

	bool occlusionCheck = false;
	bool diffuseCheck = false;
	bool specularCheck = false;
	bool emissiveCheck = false;
};

#endif __COMPONENTMATERIAL_H_

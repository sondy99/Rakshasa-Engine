#ifndef __COMPONENTMATERIAL_H_
#define __COMPONENTMATERIAL_H_

#include "Component.h"
#include "ModuleTextures.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material);
	~ComponentMaterial();

	void DrawProperties() override;
	Component* Clone() override;
public:
	Material material;
};

#endif __COMPONENTMATERIAL_H_

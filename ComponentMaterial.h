#ifndef __COMPONENTMATERIAL_H_
#define __COMPONENTMATERIAL_H_

#include "Component.h"
#include "ModuleTextures.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* gameObject, ComponentType componentType, Material material);
	~ComponentMaterial();
public:
	Material material;
};

#endif __COMPONENTMATERIAL_H_

#ifndef __COMPONENTMATERIAL_H_
#define __COMPONENTMATERIAL_H_

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* gameObject, ComponentType componentType);
	~ComponentMaterial();
};

#endif __COMPONENTMATERIAL_H_

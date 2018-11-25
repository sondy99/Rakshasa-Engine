#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* gameObject, ComponentType componentType, Material material)
	: Component(gameObject, componentType), material(material)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

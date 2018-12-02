#include "ComponentMaterial.h"

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
		ImGui::Image((ImTextureID)material.texture0, ImVec2(200, 200));
		ImGui::Text("Dimensions: %dx%d", material.width, material.height);
	}
}
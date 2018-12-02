#include "ComponentTransformation.h"

ComponentTransformation::ComponentTransformation(GameObject* gameObjectParent,
	ComponentType componentType, float3 position, float3 scale, Quat rotation) :
	Component(gameObjectParent, componentType),
	position(position), scale(scale), rotation(rotation)
{
	localMatrix = float4x4::FromTRS(position, rotation, scale);
}

ComponentTransformation::~ComponentTransformation()
{
}

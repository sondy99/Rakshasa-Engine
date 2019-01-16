#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib.h"

class ComponentTransformation : public Component
{
public:
	ComponentTransformation();
	ComponentTransformation(GameObject* gameObjectParent, ComponentType componentType, float3 position, float3 scale, Quat rotation);
	~ComponentTransformation();

	void DrawProperties() override;
	Component* Clone() override;

	void Save(const Config* config) override;
	void Load(const Config* config, rapidjson::Value& value) override;

	void UpdateLocalModelMatrix();
	void SetPositionRotationScaleFromLocalModelMatrix();
public:
	bool identity = false;

	float3 position = { 0.0f, 0.0f, 0.0f };
	float3 eulerRotation = { 0.0f, 0.0f, 0.0f };
	float3 scale = { 1.0f, 1.0f, 1.0f };
	Quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

	math::float4x4 localModelMatrix = math::float4x4::identity;
	math::float4x4 globalModelMatrix = math::float4x4::identity;
private:
	void RotationToEuler();
};

#endif __COMPONENTTRANSFORM_H__
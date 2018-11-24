//#ifndef __COMPONENTTRANSFORM_H__
//#define __COMPONENTTRANSFORM_H__
//
//#include "Component.h"
//#include "MathGeoLib.h"
//
//class ComponentTransformation : public Component
//{
//public:
//	ComponentTransformation(GameObject* gameObject, componentType componentType);
//	~ComponentTransformation();
//public:
//	float3 position = { 0.0f, 0.0f, 0.0f };
//	float3 rotationVec = { 0.0f, 0.0f, 0.0f };
//	float3 scale = { 1.0f, 1.0f, 1.0f };
//	Quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
//
//	float4x4 localMatrix = float4x4::identity;
//	float4x4 globalMatrix = float4x4::identity;
//};
//
//#endif __COMPONENTTRANSFORM_H__
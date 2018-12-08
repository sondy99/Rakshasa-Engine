#ifndef __COMPONENTCAMERA_H_
#define __COMPONENTCAMERA_H_

#include "Component.h"
#include "MathGeoLib.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* gameObjectParent, ComponentType componentType);
	~ComponentCamera();

	void DrawProperties() override;
	Component* clone() override;

	math::float4x4 ProjectionMatrix();
	math::float4x4 LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp);
	void SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight);

private:
	void InitFrustum();

	void SetHorizontalFOV(float& fovXDegrees);
	void SetVerticalFOV(float& fovYDegrees);
public:
	Frustum frustum;

	math::float3 cameraPosition = math::float3(0.0f, 3.0f, 10.0f);
	math::float3 cameraFront = math::float3(0.0f, 0.0f, -1.0f);
	math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);

	float pitch = 0.0f;
	float yaw = -90;

	float fovY = 45.0f;
	float fovX = 45.0f;

	unsigned screenWidth = SCREEN_WIDTH;
	unsigned screenHeight = SCREEN_HEIGHT;

	float screenRatio = screenWidth / screenHeight;
};

#endif __COMPONENTCAMERA_H_


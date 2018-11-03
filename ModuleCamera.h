#ifndef __MODULECAMERA_H_
#define __MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"
#include "GL/glew.h"
#include "SDL.h"

class ModuleCamera : public Module
{

	enum CameraMovement {
		Upwards,
		Downwards,
		Left,
		Right,
		Forward,
		Backwards
	};

	enum CameraRotation {
		PositivePitch,
		NegativePitch,
		PositiveYaw,
		NegativeYaw
	};

public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	math::float4x4 LookAt(math::float3& position, math::float3& front, math::float3& up);

	inline math::float4x4 GetProjectionMatrix() { return frustum.ProjectionMatrix(); }

	math::float3 position = math::float3(0.0f, 0.0f, 0.0f);
	math::float3 front = math::float3(0.0f, 0.0f, 0.0f);
	math::float3 up = math::float3(0.0f, 0.0f, 0.0f);
	Frustum frustum;
	float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;

private:
	void MoveCamera(CameraMovement cameraMovement);
	void RotateCamera(CameraRotation cameraRotation);

	float cameraSpeed = 0.01f;
	float pitch;
	float yaw;

	math::float3 forw;
	math::float3 sidew;
	math::float3 upw;

};

#endif

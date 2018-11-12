#ifndef __MODULECAMERA_H_
#define __MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"
#include "Point.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
	enum CameraMovement {
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARDS
	};

	enum CameraRotation {
		PITCH,
		YAW
	};
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	math::float4x4 ProjectionMatrix();
	math::float4x4 LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp);
	void InitFrustum();
	void SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight);

	void DrawProperties();
public:
	math::float3 cameraPosition = math::float3(0.0f, 3.0f, 10.0f);
	math::float3 cameraFront = math::float3(0.0f, 0.0f, -1.0f);
	math::float3 cameraSide = math::float3(1.0f, 1.0f, 1.0f);
	math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
private:
	void CameraMovementKeyboard();
	void CameraMovementMouse();
	void MouseUpdate(const iPoint& mousePosition);

	void MoveCamera(CameraMovement cameraSide);
	void RotateCamera(CameraMovement cameraSide);
	void SetHorizontalFOV(float& fovXDegrees);
	void SetVerticalFOV(float& fovYDegrees);
	void Zooming(bool positive);
	void FocusObject(math::float3& objectCenterPos);
private:
	Frustum frustum;
	float cameraSpeed = 15.0f;
	float rotationSpeed = 65.0f;
	float mouseSensitivity = 0.2f;
	float fov = 0.0f;

	unsigned screenWidth = SCREEN_WIDTH;
	unsigned screenHeight = SCREEN_HEIGHT;
	float screenRatio = screenWidth / screenHeight;

	float fovY = 45.0f;
	float fovX = 45.0f;
	float zoomValue = 0.0f;

	float pitch = 0.0f;
	float yaw = -90;

	bool firstMouse = true;
	int lastX = 0;
	int lastY = 0;

	math::float3 sceneCenter = math::float3(0.0f, 0.0f, 0.0f);

};

#endif __MODULECAMERA_H_
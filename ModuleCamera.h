#ifndef __MODULECAMERA_H_
#define __MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"
#include "Point.h"
#include "MathGeoLib.h"

class ComponentCamera;

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

	void SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight);

	void DrawProperties();
public:
	bool toggleCameraProperties = true;

	ComponentCamera* sceneCamera = nullptr;
	ComponentCamera* selectedCamera = nullptr;
	bool viewPortIsFocused = false;
private:
	void CameraMovementKeyboard();
	void CameraMovementMouse();
	void MouseUpdate(const iPoint& mousePosition);

	void MoveCamera(CameraMovement cameraSide);
	void RotateCamera(CameraMovement cameraSide);
	void Zooming(bool positive);
	void FocusObject(math::float3& objectCenterPos);
private:
	math::float3 sceneCenter = math::float3(0.0f, 0.0f, 0.0f);
};

#endif __MODULECAMERA_H_
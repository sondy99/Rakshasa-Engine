#ifndef __MODULECAMERA_H_
#define __MODULECAMERA_H_

#include "Module.h"

#include "Globals.h"

#include <vector>
#include <list>

#include "Point.h"
#include "MathGeoLib.h"

enum class ComponentType;
class GameObject;
class Component;
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
	void CleanUpFromList(const ComponentCamera* componentCamera);
	std::list<ComponentCamera*>::iterator CleanUpIterator(const std::list<ComponentCamera*>::iterator iterator);

	void SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight);

	void DrawProperties();
	ComponentCamera* CreateComponentCamera(GameObject* gameObjectParent, ComponentType componentType);
	void RemoveCameraComponent(const Component* componentToBeRemove); 
	void PickGameObject();
public:
	bool toggleCameraProperties = true;

	ComponentCamera* sceneCamera = nullptr;
	ComponentCamera* selectedCamera = nullptr;
	std::list<ComponentCamera*>* cameras = nullptr;
	bool isFocusedViewPort = false;
	bool isFocuseSceneWindow = false;
	bool isFocusedcomboGizmoOptions = false;
private:
	void CameraMovementKeyboard();
	void CameraMovementMouse();
	void MouseUpdate(const iPoint& mousePosition);

	void MoveCamera(CameraMovement cameraSide);
	void RotateCamera(CameraMovement cameraSide);
	void Zooming(bool positive); 
	void FitCamera(const AABB &boundingBox);
	void ManageFocus();
	void FocusSpecificPosition(const math::float3& objectCenterPos);
	void FocusUsingTransformation();
private:
	math::float3 sceneCenter = math::float3(0.0f, 0.0f, 0.0f);
	bool clickOnViewPort = false;
	std::vector<GameObject*> objectsPossiblePick;
};

#endif __MODULECAMERA_H_
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"

ModuleCamera::ModuleCamera() 
{
}

// Destructor
ModuleCamera::~ModuleCamera() 
{
}

// Called before render is available
bool ModuleCamera::Init()
{
	InitFrustum();
	return true;
}

// Called every draw update
update_status ModuleCamera::PreUpdate()
{
	CameraMovementKeyboard();
	CameraMovementMouse();

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		FocusObject(sceneCenter);
	}

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		cameraSpeed = cameraSpeed * 3;
		rotationSpeed = rotationSpeed * 3;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
		cameraSpeed = cameraSpeed / 3;
		rotationSpeed = rotationSpeed / 3;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp() {

	return true;
}

void ModuleCamera::MoveCamera(CameraMovement cameraSide) {

	float normCameraSpeed = cameraSpeed * App->deltaTime;

	switch (cameraSide) {
	case UP:
		cameraPos += cameraUp.Normalized() * normCameraSpeed;
		break;
	case DOWN:
		cameraPos -= cameraUp.Normalized() * normCameraSpeed;
		break;
	case LEFT:
		cameraPos += cameraUp.Cross(cameraFront).Normalized() * normCameraSpeed;
		break;
	case RIGHT:
		cameraPos -= cameraUp.Cross(cameraFront).Normalized() * normCameraSpeed;
		break;
	case FORWARD:
		cameraPos += cameraFront.Normalized() * normCameraSpeed;
		break;
	case BACKWARDS:
		cameraPos -= cameraFront.Normalized() * normCameraSpeed;
		break;
	}
}

void ModuleCamera::RotateCamera(CameraMovement cameraSide) {

	switch (cameraSide) {
	case UP:
		pitch += rotationSpeed * App->deltaTime;
		break;
	case DOWN:
		pitch -= rotationSpeed * App->deltaTime;
		break;
	case LEFT:
		yaw -= rotationSpeed * App->deltaTime;
		break;
	case RIGHT:
		yaw += rotationSpeed * App->deltaTime;
		break;
	}

	pitch = math::Clamp(pitch, -80.0f, 80.0f);

	math::float3 rotation;
	rotation.x = SDL_cosf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	rotation.y = SDL_sinf(math::DegToRad(pitch));
	rotation.z = SDL_sinf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	cameraFront = rotation.Normalized();
}

math::float4x4 ModuleCamera::LookAt(math::float3& cameraPos, math::float3& cameraFront, math::float3& cameraUp) {
	cameraFront.Normalize();
	math::float3 s(cameraFront.Cross(cameraUp)); s.Normalize();
	math::float3 u(s.Cross(cameraFront));

	math::float4x4 matrix;
	matrix[0][0] = s.x; matrix[0][1] = s.y; matrix[0][2] = s.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -cameraFront.x; matrix[2][1] = -cameraFront.y; matrix[2][2] = -cameraFront.z;
	matrix[0][3] = -s.Dot(cameraPos); matrix[1][3] = -u.Dot(cameraPos); matrix[2][3] = cameraFront.Dot(cameraPos);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;

	return matrix;
}

math::float4x4 ModuleCamera::ProjectionMatrix() {
	return frustum.ProjectionMatrix();
}

void ModuleCamera::InitFrustum() {
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	SetVerticalFOV(fovY);
}

void ModuleCamera::SetHorizontalFOV(float& fovXDegrees) {
	fovX = fovXDegrees;
	frustum.horizontalFov = math::DegToRad(fovX);
	frustum.verticalFov = 2.0f * atanf(tanf(frustum.horizontalFov * 0.5) * ((float)screenHeight / (float)screenWidth));
}

void ModuleCamera::SetVerticalFOV(float& fovYDegrees) {
	fovY = fovYDegrees;
	frustum.verticalFov = math::DegToRad(fovY);
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}

void ModuleCamera::SetScreenNewScreenSize(unsigned newWidth, unsigned newHeight) {
	screenWidth = newWidth;
	screenHeight = newHeight;
	screenRatio = (float)screenWidth / (float)screenHeight;

	SetHorizontalFOV(fovX);
	SetVerticalFOV(fovY);
}

void ModuleCamera::MouseUpdate(const iPoint& mousePosition)
{
	if (firstMouse) {
		lastX = mousePosition.x;
		lastY = mousePosition.y;
		firstMouse = false;
	}

	float xoffset = mousePosition.x - lastX;
	float yoffset = lastY - mousePosition.y;
	lastX = mousePosition.x;
	lastY = mousePosition.y;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	pitch = math::Clamp(pitch, -80.0f, 80.0f);

	math::float3 rotation;
	rotation.x = SDL_cosf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	rotation.y = SDL_sinf(math::DegToRad(pitch));
	rotation.z = SDL_sinf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	cameraFront = rotation.Normalized();
}

void ModuleCamera::Zooming(bool positive) {
	if (positive)
		fovX += 10.0;
	else
		fovX -= 10.0;

	fovX = math::Clamp(fovX, 0.0f, 100.0f);

	zoomValue = 45.0f / fovX;

	SetHorizontalFOV(fovX);
	
}

void ModuleCamera::FocusObject(math::float3& objectCenterPos) {
	cameraFront = objectCenterPos - cameraPos;
	pitch = math::RadToDeg(SDL_tanf(cameraFront.y / cameraFront.x));
	yaw = math::RadToDeg(SDL_tanf(cameraFront.z / cameraFront.x)) - 90;
}

void ModuleCamera::CameraMovementMouse() {
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		SDL_ShowCursor(SDL_DISABLE);
		MouseUpdate(App->input->GetMousePosition());
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		SDL_ShowCursor(SDL_ENABLE);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN) {
		Zooming(true);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN) {
		Zooming(false);
	}
}

void ModuleCamera::CameraMovementKeyboard() {
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) {
		MoveCamera(UP);
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
		MoveCamera(DOWN);
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		MoveCamera(LEFT);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		MoveCamera(RIGHT);
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		MoveCamera(FORWARD);
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		MoveCamera(BACKWARDS);
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		RotateCamera(UP);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		RotateCamera(DOWN);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		RotateCamera(LEFT);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		RotateCamera(RIGHT);
	}
}
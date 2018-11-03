#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"

ModuleCamera::ModuleCamera() :
	position(math::float3(0.0f, 3.0f, 10.0f)),
	front(math::float3(0.0f, 0.0f, 0.0f)),
	up(math::float3(0.0f, 1.0f, 0.0f)),
	pitch(0.0f),
	yaw(0.0f)
{
}

// Destructor
ModuleCamera::~ModuleCamera() {}

bool ModuleCamera::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (SCREEN_WIDTH / SCREEN_HEIGHT));

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		MoveCamera(Upwards);
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		MoveCamera(Downwards);
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		MoveCamera(Left);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		MoveCamera(Right);
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		MoveCamera(Forward);
	}
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		MoveCamera(Backwards);
	}
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		RotateCamera(NegativePitch);
	}
	else if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		RotateCamera(PositivePitch);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		RotateCamera(PositiveYaw);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		RotateCamera(NegativeYaw);
	}

	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

math::float4x4 ModuleCamera::LookAt(math::float3& position, math::float3& front, math::float3& up)
{
	math::float4x4 matrix;

	forw = math::float3(front - position); forw.Normalize();
	sidew = math::float3(forw.Cross(up)); sidew.Normalize();
	upw = math::float3(sidew.Cross(forw));

	matrix[0][0] = sidew.x; matrix[0][1] = sidew.y; matrix[0][2] = sidew.z;
	matrix[1][0] = upw.x; matrix[1][1] = upw.y; matrix[1][2] = upw.z;
	matrix[2][0] = -forw.x; matrix[2][1] = -forw.y; matrix[2][2] = -forw.z;
	matrix[0][3] = -sidew.Dot(position); matrix[1][3] = -upw.Dot(position); matrix[2][3] = forw.Dot(position);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;

	return matrix;
}

void ModuleCamera::MoveCamera(CameraMovement cameraMovement)
{
	switch (cameraMovement)
	{
	case Upwards:
		position += math::float3(0.0f, 1.0f, 0.0f) * cameraSpeed;
		front += math::float3(0.0f, 1.0f, 0.0f) * cameraSpeed;
		break;
	case Downwards:
		position -= math::float3(0.0f, 1.0f, 0.0f) * cameraSpeed;
		front -= math::float3(0.0f, 1.0f, 0.0f) * cameraSpeed;
		break;
	case Left:
		position -= sidew * cameraSpeed;
		front -= sidew * cameraSpeed;
		break;
	case Right:
		position += sidew * cameraSpeed;
		front += sidew * cameraSpeed;
		break;
	case Forward:
		position += forw * cameraSpeed;
		front += forw * cameraSpeed;
		break;
	case Backwards:
		position -= forw * cameraSpeed;
		front -= forw * cameraSpeed;
		break;
	}
}

void ModuleCamera::RotateCamera(CameraRotation cameraRotation) {
	math::float3 direction;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	switch (cameraRotation) {
	case PositivePitch:
		pitch += cameraSpeed;
		forw.y = SDL_sin(degreesToRadians(pitch));
		forw.x = SDL_cos(degreesToRadians(pitch));
		forw.z = SDL_cos(degreesToRadians(pitch));
		forw.Normalize();
		front += forw * cameraSpeed;
		break;
	case NegativePitch:
		pitch -= cameraSpeed * 2;
		forw.y = SDL_sin(degreesToRadians(pitch));
		forw.x = SDL_cos(degreesToRadians(pitch));
		forw.z = SDL_cos(degreesToRadians(pitch));
		forw.Normalize();
		front -= forw * cameraSpeed;
		break;
	case PositiveYaw:
		yaw += cameraSpeed;
		forw.y = SDL_cos(degreesToRadians(pitch)) * SDL_cos(degreesToRadians(yaw));
		forw.x = SDL_sin(degreesToRadians(pitch));
		forw.z = SDL_cos(degreesToRadians(pitch)) * SDL_sin(degreesToRadians(yaw));
		forw.Normalize();
		front += forw * cameraSpeed;
		position += forw * cameraSpeed;
		break;
	case NegativeYaw:
		LOG("%f", yaw);
		yaw -= cameraSpeed;
		forw.y = SDL_cos(degreesToRadians(pitch)) * SDL_cos(degreesToRadians(yaw));
		forw.x = SDL_sin(degreesToRadians(pitch));
		forw.z = SDL_cos(degreesToRadians(pitch)) * SDL_sin(degreesToRadians(yaw));
		forw.Normalize();
		front -= forw * cameraSpeed;
		position -= forw * cameraSpeed;
		break;
	}
}

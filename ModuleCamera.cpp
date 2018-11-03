#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"

ModuleCamera::ModuleCamera() : 
	position(math::float3(0.0f, 0.0f, 5.0f)),
	front(math::float3(0.0f, 0.0f, -1.0f)),
	up(math::float3(0.0f, 1.0f, 0.0f)) 
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
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp() {

	return true;
}

math::float4x4 ModuleCamera::LookAt(math::float3& cameraPos, math::float3& front, math::float3& up) {
	front.Normalize();
	math::float3 s(front.Cross(up)); s.Normalize();
	math::float3 u(s.Cross(front));

	math::float4x4 matrix;
	matrix[0][0] = s.x; matrix[0][1] = s.y; matrix[0][2] = s.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -front.x; matrix[2][1] = -front.y; matrix[2][2] = -front.z;
	matrix[0][3] = -s.Dot(cameraPos); matrix[1][3] = -u.Dot(cameraPos); matrix[2][3] = front.Dot(cameraPos);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;

	return matrix;
}

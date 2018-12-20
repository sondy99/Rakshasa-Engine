#include "ModuleEnvironment.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "debugdraw.h"
#include "MathGeoLib.h"

#include "ComponentCamera.h"
#include "GameObject.h"

#include "GL/glew.h"
#include "SDL.h"

#include "MathGeoLib.h"

ModuleEnvironment::ModuleEnvironment()
{
}

ModuleEnvironment::~ModuleEnvironment()
{
}

bool ModuleEnvironment::Init()
{
	App->shader->LoadShaders(App->shader->colorProgram, "colorProgram.vs", "colorProgram.fs");

	return true;
}

void ModuleEnvironment::DrawReferenceGround() 
{
	dd::xzSquareGrid(-400.0f, 400.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));
}

void ModuleEnvironment::DrawReferenceAxis() 
{
	float axis_size = 1.0f;
	dd::axisTriad(math::float4x4::identity, axis_size*0.125f, axis_size*1.25f, 0, false);
}

void ModuleEnvironment::DrawBoundingBox(GameObject* gameObject)
{
	dd::aabb(gameObject->globalBoundingBox.minPoint, gameObject->globalBoundingBox.maxPoint, dd::colors::Yellow);
}

void ModuleEnvironment::DrawFrustum(ComponentCamera* camera)
{
	dd::frustum((camera->frustum.ProjectionMatrix() * camera->frustum.ViewMatrix()).Inverted(), dd::colors::Cyan);
}
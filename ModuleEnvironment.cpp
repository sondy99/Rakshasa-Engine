#include "ModuleEnvironment.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "debugdraw.h"
#include "MathGeoLib.h"

#include "ComponentCamera.h"
#include "ComponentMesh.h"

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
	dd::xzSquareGrid(-40000.0f, 40000.0f, 0.0f, 100.0f, math::float3(0.65f, 0.65f, 0.65f));
}

void ModuleEnvironment::DrawReferenceAxis() 
{
	float axis_size = 100.0f;
	dd::axisTriad(math::float4x4::identity, axis_size*0.125f, axis_size*1.25f, 0, false);
}

void ModuleEnvironment::DrawBoundingBox(const ComponentMesh& componentMesh)
{
	dd::aabb(componentMesh.globalBoundingBox.minPoint, componentMesh.globalBoundingBox.maxPoint, dd::colors::Yellow);
}

void ModuleEnvironment::DrawBoundingBox(math::float3 minPoint, math::float3 maxPoint)
{
	dd::aabb(minPoint, maxPoint, dd::colors::Blue);
}

void ModuleEnvironment::DrawFrustum(const ComponentCamera& camera)
{
	dd::frustum((camera.frustum.ProjectionMatrix() * camera.frustum.ViewMatrix()).Inverted(), dd::colors::Cyan);
}
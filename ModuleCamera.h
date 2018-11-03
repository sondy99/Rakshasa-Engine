#ifndef __MODULECAMERA_H_
#define __MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"
#include "GL/glew.h"
#include "SDL.h"

class ModuleCamera : public Module
{

public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status	PreUpdate();
	bool			CleanUp();

	math::float4x4 LookAt(math::float3& position, math::float3& front, math::float3& up);

	inline math::float4x4 GetProjectionMatrix() { return frustum.ProjectionMatrix(); }
	
public:
	math::float3 position;
	math::float3 front;
	math::float3 up;
	Frustum frustum;
	float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
};

#endif

#ifndef __ModuleEnvironment_H__
#define __ModuleEnvironment_H__

#include "Module.h"

#include "MathGeoLib.h"

class ComponentCamera;
class ComponentMesh;

class ModuleEnvironment : public Module
{
public:
	ModuleEnvironment();
	~ModuleEnvironment();

	bool Init() override;

	void DrawReferenceGround();
	void DrawReferenceAxis();
	void DrawBoundingBox(const ComponentMesh& componentMesh);
	void DrawBoundingBox(math::float3 minPoint, math::float3 maxPoint);
	void DrawFrustum(const ComponentCamera& camera);
};

#endif __ModuleEnvironment_H__



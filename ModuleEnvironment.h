#ifndef __ModuleEnvironment_H__
#define __ModuleEnvironment_H__

#include "Module.h"

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
	void DrawFrustum(ComponentCamera* camera);
	void DrawBoundingBox(ComponentMesh* mesh);
};

#endif __ModuleEnvironment_H__



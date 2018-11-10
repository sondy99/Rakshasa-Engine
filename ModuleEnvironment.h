#ifndef __ModuleEnvironment_H__
#define __ModuleEnvironment_H__

#include "Module.h"

class ModuleEnvironment : public Module
{
public:
	ModuleEnvironment();
	~ModuleEnvironment();

	void DrawReferenceGround();
	void DrawReferenceAxis();
};

#endif __ModuleEnvironment_H__



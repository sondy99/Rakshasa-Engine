#ifndef __MODULERENDEREXERCISE_H__
#define __MODULERENDEREXERCISE_H__

#include "Module.h"

class ModuleRenderTriangle : public Module
{
public:
	ModuleRenderTriangle();
    ~ModuleRenderTriangle();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

private:
    unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned texture0 = 0;
};

#endif __MODULERENDEREXERCISE_H__

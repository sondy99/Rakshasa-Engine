#ifndef __ModuleRenderExercise_h__
#define __ModuleRenderExercise_h__

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

#endif /* __ModuleRenderExercise_h__ */

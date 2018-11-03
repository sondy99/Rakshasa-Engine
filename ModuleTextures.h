#ifndef __ModuleTextures_H__
#define __ModuleTextures_H__

#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

struct SDL_Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	GLuint const Load(const char* path);
	void Unload(unsigned id);

private:
};

#endif // __ModuleTextures_H__
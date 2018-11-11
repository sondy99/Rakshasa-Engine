#ifndef __APLICATION_H_
#define __APLICATION_H_

#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleCamera;
class ModuleShader;
class ModuleTextures;
class ModuleEnvironment;
class ModuleModelLoader;
class ModuleEditor;

//class ModuleRenderTriangle;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleShader* shader = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleEnvironment* environment = nullptr;
	ModuleModelLoader* modelLoader = nullptr;
	ModuleEditor* editor = nullptr;

	//ModuleRenderTriangle* renderTrieangle = nullptr; 
private:
	std::list<Module*> modules;
};

extern Application* App;

#endif __APLICATION_H_
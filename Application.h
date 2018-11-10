#pragma once

#include<list>
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

class ModuleRenderTriangle;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void Tick();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleShader* shader = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleEnvironment* environment = nullptr;
	ModuleModelLoader* modelLoader = nullptr;

	ModuleRenderTriangle* renderTrieangle = nullptr;

	float lastTickTime = 0.0f;
	float deltaTime = 0.0f;
	float auxTimer = 0.0f;
	int frameCounter = 0;
	int FPS = 0;
private:

	std::list<Module*> modules;

};

extern Application* App;

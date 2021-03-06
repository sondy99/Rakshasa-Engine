#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "Moduleshader.h"
#include "ModuleTextures.h"
#include "ModuleEnvironment.h"
#include "ModuleModelLoader.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleLibrary.h"
#include "ModuleTime.h"

//#include "ModuleRenderTriangle.h"

using namespace std;

Application::Application()
{
	modules.push_back(fileSystem = new ModuleFileSystem());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(input = new ModuleInput());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(shader = new ModuleShader());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(environment = new ModuleEnvironment());
	modules.push_back(modelLoader = new ModuleModelLoader());
	modules.push_back(debugDraw = new ModuleDebugDraw());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(time = new ModuleTime());
	modules.push_back(library = new ModuleLibrary());

	//modules.push_back(renderTrieangle = new ModuleRenderTriangle());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	BROFILER_FRAME("MainLoop");

	update_status ret = UPDATE_CONTINUE;
	
	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
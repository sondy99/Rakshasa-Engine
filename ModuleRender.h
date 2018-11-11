#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"

#include <vector>

#include "ModuleModelLoader.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	
	void WindowResized(unsigned width, unsigned height);
	void RenderMesh(const Mesh& mesh, 
		const Material& material, 
		unsigned program, const math::float4x4& model,
		const math::float4x4& view, const math::float4x4& proj);
	void DrawProperties();
public:
	void* context = nullptr;

	float deltaTime = 0.0f;
private:
	void FpsCount();
	void manageFpsAndMsList();
private:
	float lastTickTime = 0.0f;
	float auxTimer = 0.0f;
	int frameCounter = 0;
	float fps = 0;
	unsigned ticksNow = 0;
	
	float minFps = 100.0f;
	float maxMs = 0.0f;

	std::vector<float> fpsList;
	std::vector<float> msList;
};

#endif __MODULERENDER_H__
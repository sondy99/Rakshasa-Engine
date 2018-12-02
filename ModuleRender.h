#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"

#include <vector>

#include "ModuleModelLoader.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
struct Mesh;
struct Material;

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
	
	void RenderMesh(const Mesh& mesh, 
		const Material& material, 
		unsigned program, const math::float4x4& model,
		const math::float4x4& view, const math::float4x4& proj);
	void DrawProperties();
	void DrawSceneWindow();
public:
	void* context = nullptr;
	float deltaTime = 0.0f;

	bool toggleRenderProperties = true;
private:
	void FpsCount();
	void manageFpsAndMsList();
	void InitFrameBuffer(int width, int height);
	void RenderComponentFromGameObject(GameObject* gameObject, math::float4x4 view, math::float4x4 projection);
	void CalculateGameObjectGlobalMatrix(GameObject* gameObject);
private:
	unsigned frameBufferObject = 0u;
	unsigned renderBufferObject = 0u;
	unsigned renderTexture = 0u;
	bool sceneEnabled = true;

	float lastTickTime = 0.0f;
	float auxTimer = 0.0f;
	int frameCounter = 0;
	float fps = 0;
	unsigned ticksNow = 0u;
	
	float minFps = 100.0f;
	float maxMs = 0.0f;

	std::vector<float> fpsList;
	std::vector<float> msList;
};

#endif __MODULERENDER_H__
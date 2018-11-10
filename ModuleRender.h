#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"

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
	void RenderMesh(const ModuleModelLoader::Mesh& mesh, 
		const ModuleModelLoader::Material& material, 
		unsigned program, const math::float4x4& model,
		const math::float4x4& view, const math::float4x4& proj);
private:
	void* context = nullptr;
};

#endif __MODULERENDER_H__
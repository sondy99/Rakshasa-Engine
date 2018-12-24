#ifndef __ModuleTextures_H__
#define __ModuleTextures_H__

#include "Module.h"
#include "ModuleModelLoader.h"

#include "Globals.h"
#include "GL/glew.h"

#include "string"
#include<list>

enum class ComponentType;
class GameObject;
class Component;
class ComponentMaterial;

struct SDL_Texture;
struct aiMesh;
struct aiMaterial;

struct Mesh
{
	unsigned vao = 0;
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned verticesNumber = 0;
	std::vector<float3> vertices;
	unsigned indicesNumber = 0;
	unsigned normalsOffset = 0;
	unsigned texturesOffset = 0;
	unsigned vertexSize = 0;
};

struct Material
{
	int width = 0;
	int height = 0;
	unsigned texture0 = 0;
	math::float4 color = math::float4::zero;
	aiMaterial* material = nullptr;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;

	Material LoadMaterialFromFBX(std::string path);
	Material LoadMaterial(std::string path);
	void Unload(unsigned id); 
	void ReplaceMaterial(const char* path, ComponentMaterial* componentMaterial);
	ComponentMaterial* CreateComponentMaterial();
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material);
	void RemoveMaterialComponent(Component* componentToBeRemove);
	void RemoveMaterial(ComponentMaterial* componentMaterial);
private:
	std::list<ComponentMaterial*> materials;
};

#endif __ModuleTextures_H__
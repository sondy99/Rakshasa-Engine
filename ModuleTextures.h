#ifndef __ModuleTextures_H__
#define __ModuleTextures_H__

#include "Module.h"
#include "ModuleModelLoader.h"

#include "Globals.h"
#include "GL/glew.h"

#include "string"
#include<list>

enum class MaterialTypeSelected;
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

	float* normals = nullptr;
	float* colors = nullptr;
	float* uvs = nullptr;
	unsigned verticesNumber = 0;
	float* vertices = nullptr;
	unsigned indicesNumber = 0;
	unsigned* indices = nullptr;

	unsigned normalsOffset = 0;
	unsigned texturesOffset = 0;
	unsigned vertexSize = 0;
};

struct Material
{
	unsigned occlusionMap = 0u; //ambient
	unsigned occlusionFallback = 0u;
	float ambientK = 0.5f;
	int ambientWidth = 0;
	int ambientHeight = 0;

	unsigned diffuseMap = 0u;
	math::float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	unsigned diffuseFallback = 0u;
	float diffuseK = 1.0f;
	int diffuseWidth = 0;
	int diffuseHeight = 0;

	unsigned specularMap = 0u;
	math::float4 specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	unsigned specularFallback = 0u;
	float specularK = 0.6f;
	float shininess = 64.0f;
	int specularWidth = 0;
	int specularHeight = 0;

	unsigned emissiveMap = 0u;
	math::float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	unsigned emissiveFallback = 0u;
	int emissiveWidth = 0;
	int emissiveHeight = 0;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;
	bool CleanUp() override;
	void CleanUpFromList(ComponentMaterial* componentMaterial);
	std::list<ComponentMaterial*>::iterator CleanUpIterator(std::list<ComponentMaterial*>::iterator iterator);

	void LoadMaterial(std::string path, unsigned& textureID, int& width, int& height);
	void LoadMaterial(const char* path, ComponentMaterial* componentMaterial, MaterialTypeSelected materialTypeSelected);
	void Unload(unsigned id); 
	ComponentMaterial* CreateComponentMaterial();
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material);
	void RemoveMaterialComponent(Component* componentToBeRemove);
	void RemoveMaterial(ComponentMaterial* componentMaterial);
private:
	std::list<ComponentMaterial*> materials;
};

#endif __ModuleTextures_H__
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
	unsigned occlusionMap = 0u; //ambient
	float ambientK = 0.5f;
	int ambientWidth = 0;
	int ambientHeight = 0;

	unsigned diffuseMap = 0u;
	math::float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float diffuseK = 1.0f;
	int diffuseWidth = 0;
	int diffuseHeight = 0;

	unsigned specularMap = 0u;
	math::float4 specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float specularK = 0.6f;
	float shininess = 64.0f;
	int specularWidth = 0;
	int specularHeight = 0;

	unsigned emissiveMap = 0u;
	math::float4 emissiveColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int emissiveWidth = 0;
	int emissiveHeight = 0;




	int width = 0;
	int height = 0;
	unsigned texture0 = 0u;
	math::float4 color = math::float4::zero;
	aiMaterial* material = nullptr;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;

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
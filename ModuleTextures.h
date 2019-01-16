#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

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

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init() override;
	bool CleanUp() override;
	void CleanUpFromList(const ComponentMaterial* componentMaterial);
	std::list<ComponentMaterial*>::iterator CleanUpIterator(const std::list<ComponentMaterial*>::iterator iterator);

	void LoadMaterial(const char* path, unsigned& textureID, int& width, int& height);
	void LoadMaterial(const char* path, ComponentMaterial* componentMaterial, MaterialTypeSelected materialTypeSelected);
	void Unload(unsigned id); 
	ComponentMaterial* CreateComponentMaterial();
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMaterial* CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material);
	void RemoveMaterialComponent(const Component* componentToBeRemove);
	void RemoveMaterial(ComponentMaterial* componentMaterial);
private:
	std::list<ComponentMaterial*> materials;
};

#endif __MODULETEXTURES_H__
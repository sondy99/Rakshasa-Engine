#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"

#include <vector>
#include <list>

#include "ModuleModelLoader.h"

#include "MathGeoLib.h"

class GameObject;
class QuadtreeNode;

enum class MaterialTypeSelected;
enum class ComponentType;
class Component;
class ComponentCamera;
class ComponentMesh;
class ComponentMaterial;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
struct Mesh;
struct Material;

enum class FrameBufferType
{
	SCENE,
	GAME
};

struct FrameBufferStruct
{
	unsigned frameBufferObject = 0u;
	unsigned renderBufferObject = 0u;
	unsigned renderTexture = 0u;
	FrameBufferType frameBufferType;
};

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
	void CleanUpFromList(const ComponentMesh* componentMesh);
	std::list<ComponentMesh*>::iterator CleanUpIterator(const std::list<ComponentMesh*>::iterator iterator);

	void RenderMesh(const ComponentMesh& componentMesh, const ComponentMaterial* componentMaterial,
		const math::float4x4& model, const math::float4x4& view, const math::float4x4& proj);
	
	void DrawProperties();
	void DrawCameraSceneWindow();
	void DrawCameraGameWindow();
	ComponentMesh* CreateComponentMesh();
	ComponentMesh* CreateComponentMesh(GameObject* gameObjectParent, ComponentType componentType);
	ComponentMesh* CreateComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh* mesh);
	void RemoveMeshComponent(const Component* componentToBeRemove);
	void LoadQuadTreeForAllMesh();

	void DrawImGuizmo(float sceneWidth, float sceneHeight);
public:
	void* context = nullptr;
	float deltaTime = 0.0f;

	bool toggleRenderProperties = true;
	ComponentCamera* componentCameraGameSelected = nullptr;
	float sceneViewportX = 0.0f;
	float sceneViewportY = 0.0f;
	std::list<ComponentMesh*> meshes; 
	bool enableVSync = true;
private:
	void FpsCount();
	void ManageFpsAndMsList();
	void InitFrameBuffer(int width, int height, FrameBufferStruct &frameBufferToInit);
	void RenderComponentFromMeshesList(ComponentMesh* componentMesh, math::float4x4 view, math::float4x4 projection, FrameBufferType frameBufferType);
	void RenderComponentUsingQuadTree(math::float4x4 view, math::float4x4 projection, FrameBufferType frameBufferType);
	void CalculateGameObjectGlobalMatrix(GameObject* gameObject);
	void RenderUsingSpecificFrameBuffer(FrameBufferStruct frameBufferToRender, const ComponentCamera* camera, math::float4x4 view, math::float4x4 projection);
	void ManageComboBoxCamera(); 
	void DrawQuadTreeNode(const QuadtreeNode* quadtreeNode);
	void GenerateFallback();
private:
	FrameBufferStruct frameBufferScene;
	FrameBufferStruct frameBufferGame;
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
	std::vector<GameObject*> gameObjectsCollideQuadtree;

	unsigned fallback = 0u;
};

#endif __MODULERENDER_H__
#include "Globals.h"

#include "Application.h"

#include "ModuleRender.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransformation.h"

#include "GL/glew.h"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "MathGeoLib.h"

#include <string>

#include "ImporterMesh.h"

#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838)  

#define PAR_SHAPES_IMPLEMENTATION
#include "par_shapes.h"
#pragma warning(pop)

ModuleModelLoader::ModuleModelLoader()
{
}

ModuleModelLoader::~ModuleModelLoader()
{
}

bool ModuleModelLoader::Init()
{
	return true;
}

void ModuleModelLoader::LoadGeometry(GameObject* gameObjectParent, GeometryType geometryType)
{
	par_shapes_mesh_s* parMesh = nullptr;

	switch (geometryType)
	{
	case GeometryType::SPHERE:
		parMesh = par_shapes_create_parametric_sphere(30, 30);
		break;
	case GeometryType::TORUS:
		parMesh = par_shapes_create_torus(30, 30, 0.5f);
		break;
	case GeometryType::CYLINDER:
		parMesh = par_shapes_create_cylinder(30, 30);
		break;
	case GeometryType::PLANE:
		parMesh = par_shapes_create_plane(30, 30);
		break;
	case GeometryType::CUBE:
		parMesh = par_shapes_create_cube();
		break;
	}

	if (parMesh != nullptr && parMesh)
	{
		par_shapes_scale(parMesh, 100.0f, 100.0f, 100.0f);

		CreateTransformationComponent(gameObjectParent);

		math::float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		CreateMeshComponent(parMesh, gameObjectParent, color);

		par_shapes_free_mesh(parMesh);
	}
}

bool ModuleModelLoader::CleanUp()
{
	CleanUpMeshesAndTextures(App->scene->root);

	return true;
}

void ModuleModelLoader::CleanUpMeshesAndTextures(const GameObject * gameObject)
{
	for (auto &gameObjectChild : gameObject->childrens)
	{
		if (gameObjectChild->childrens.size() > 0)
		{
			//TODO: change this to not use recursivity
			CleanUpMeshesAndTextures(gameObjectChild);
		}

		if (gameObjectChild->components.size() > 0)
		{
			Material* material = nullptr;

			for (auto &component : gameObjectChild->components)
			{
				if (component->componentType == ComponentType::MATERIAL)
				{
					material = &(dynamic_cast<ComponentMaterial*>(component))->material;
					App->textures->Unload(material->diffuseMap);
					App->textures->Unload(material->occlusionMap);
					App->textures->Unload(material->specularMap);
					App->textures->Unload(material->emissiveMap);
				}
			}

			for (auto &component : gameObjectChild->components)
			{
				if (component->componentType == ComponentType::MESH)
				{
					Mesh& mesh = (dynamic_cast<ComponentMesh*>(component)->mesh);

					if (mesh.vbo != 0)
					{
						glDeleteBuffers(1, &mesh.vbo);
					}

					if (mesh.ibo != 0)
					{
						glDeleteBuffers(1, &mesh.ibo);
					}
				}
			}
		}
	}
}

void ModuleModelLoader::CleanUpMesh(Mesh meshStruct)
{
	if (meshStruct.vbo != 0)
	{
		glDeleteBuffers(1, &meshStruct.vbo);
	}

	if (meshStruct.ibo != 0)
	{
		glDeleteBuffers(1, &meshStruct.ibo);
	}
}

void ModuleModelLoader::GenerateMesh(Mesh& meshStruct)
{
	glGenBuffers(1, &meshStruct.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, meshStruct.vbo);

	unsigned offset = sizeof(math::float3);

	if (meshStruct.normals != nullptr)
	{
		meshStruct.normalsOffset = offset;
		offset += sizeof(math::float3);
	}

	if (meshStruct.uvs != nullptr)
	{
		meshStruct.texturesOffset = offset;
		offset += sizeof(math::float2);
	}

	meshStruct.vertexSize = offset;

	glBufferData(GL_ARRAY_BUFFER, meshStruct.vertexSize * meshStruct.verticesNumber, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * meshStruct.verticesNumber, meshStruct.vertices);

	if (meshStruct.normals != nullptr)
	{
		glBufferSubData(GL_ARRAY_BUFFER, meshStruct.normalsOffset * meshStruct.verticesNumber, sizeof(float) * 3 * meshStruct.verticesNumber, meshStruct.normals);
	}

	if (meshStruct.uvs != nullptr)
	{
		glBufferSubData(GL_ARRAY_BUFFER, meshStruct.texturesOffset * meshStruct.verticesNumber, sizeof(float2)*meshStruct.verticesNumber, meshStruct.uvs);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &meshStruct.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshStruct.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshStruct.indicesNumber * sizeof(unsigned), meshStruct.indices, GL_STATIC_DRAW);

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	GenerateVAO(meshStruct);
}

void ModuleModelLoader::GenerateMesh(const par_shapes_mesh_s* parShapeMesh, Mesh& meshStruct)
{
	assert(parShapeMesh != NULL);
	
	glGenBuffers(1, &meshStruct.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, meshStruct.vbo);

	unsigned offset = sizeof(math::float3);

	if (parShapeMesh->normals)
	{
		meshStruct.normalsOffset = offset;
		offset += sizeof(math::float3);
	}

	meshStruct.vertexSize = offset;
	 
	glBufferData(GL_ARRAY_BUFFER, meshStruct.vertexSize * parShapeMesh->npoints, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::float3)*parShapeMesh->npoints, parShapeMesh->points);

	if (parShapeMesh->normals)
	{
		glBufferSubData(GL_ARRAY_BUFFER, meshStruct.normalsOffset * parShapeMesh->npoints, sizeof(math::float3)*parShapeMesh->npoints, parShapeMesh->normals);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &meshStruct.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshStruct.ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*parShapeMesh->ntriangles * 3, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
		sizeof(unsigned)*parShapeMesh->ntriangles * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i< unsigned(parShapeMesh->ntriangles * 3); ++i)
	{
		*(indices++) = parShapeMesh->triangles[i];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	meshStruct.verticesNumber = parShapeMesh->npoints;
	meshStruct.indicesNumber = parShapeMesh->ntriangles * 3;
	
	GenerateVAO(meshStruct);
}

void ModuleModelLoader::CreateMeshComponent(const par_shapes_mesh_s * parShapeMesh, GameObject * gameObjectMesh, const math::float4& color)
{
	Mesh meshStruct;
	GenerateMesh(parShapeMesh, meshStruct);

	gameObjectMesh->components.push_back(App->renderer->CreateComponentMesh(gameObjectMesh, ComponentType::MESH, meshStruct));

	CreateMaterialComponent(gameObjectMesh, color);
}

void ModuleModelLoader::CreateMaterialComponent(GameObject * gameObjectMesh, const math::float4& color)
{
	Material materialStruct;

	gameObjectMesh->components.push_back(App->textures->CreateComponentMaterial(gameObjectMesh, ComponentType::MATERIAL, materialStruct));
}

void ModuleModelLoader::CreateTransformationComponent(GameObject * gameObject)
{
	float3 position = { 0.0f, 0.0f, 0.0f };
	float3 scale = { 1.0f, 1.0f, 1.0f };
	Quat quatRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);

	gameObject->components.push_back(new ComponentTransformation(gameObject, ComponentType::TRANSFORMATION, position, scale, quatRotation));
}

void ModuleModelLoader::GenerateVAO(Mesh& mesh)
{
	glGenVertexArrays(1, &mesh.vao);

	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	if (mesh.normalsOffset != 0)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.normalsOffset * mesh.verticesNumber));
	}

	if (mesh.texturesOffset != 0)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.texturesOffset * mesh.verticesNumber));
	}

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleModelLoader::DrawProperties()
{
	if (toggleModelProperties)
	{
		ImGui::Begin("Model", &toggleModelProperties);
		ImGui::End();
	}
}


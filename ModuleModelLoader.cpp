#include "Globals.h"

#include "Application.h"

#include "ModuleRender.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

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

void ModuleModelLoader::LoadMaterialFromFBX(const char* filePath, GameObject* gameObjectParent)
{
	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene)
	{
		gameObjectParent->name = std::string(scene->mRootNode->mName.C_Str());

		CreateTransformationComponent(scene->mRootNode, gameObjectParent);

		CreateGameObjectsFromNode(scene, scene->mRootNode, gameObjectParent);
		
		aiReleaseImport(scene);
	}
	else
	{
		LOG(aiGetErrorString());
	}

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
					App->textures->Unload(material->texture0);
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

void ModuleModelLoader::GenerateMesh(Mesh& meshStruct, aiMesh* mesh)
{
	glGenBuffers(1, &meshStruct.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, meshStruct.vbo);

	unsigned offset = sizeof(math::float3);

	if (mesh->HasNormals())
	{
		meshStruct.normalsOffset = offset;
		offset += sizeof(math::float3);
	}

	if (mesh->HasTextureCoords(0))
	{
		meshStruct.texturesOffset = offset;
		offset += sizeof(math::float2);
	}

	meshStruct.vertexSize = offset;

	glBufferData(GL_ARRAY_BUFFER, meshStruct.vertexSize * mesh->mNumVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices);


	if (mesh->HasNormals())
	{
		glBufferSubData(GL_ARRAY_BUFFER, meshStruct.normalsOffset * mesh->mNumVertices, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals);
	}

	math::float2* texture_coords = (math::float2*)glMapBufferRange(GL_ARRAY_BUFFER, meshStruct.texturesOffset * mesh->mNumVertices, sizeof(float) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		texture_coords[i] = math::float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &meshStruct.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshStruct.ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*mesh->mNumFaces * 3, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
		sizeof(unsigned)*mesh->mNumFaces * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		assert(mesh->mFaces[i].mNumIndices == 3);

		*(indices++) = mesh->mFaces[i].mIndices[0];
		*(indices++) = mesh->mFaces[i].mIndices[1];
		*(indices++) = mesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	meshStruct.material = mesh->mMaterialIndex;
	meshStruct.verticesNumber = mesh->mNumVertices;
	meshStruct.indicesNumber = mesh->mNumFaces * 3;

	meshStruct.vertices.resize(meshStruct.verticesNumber);
	aiVector3D* auxVerticesPointer = mesh->mVertices;
	for (unsigned i = 0u; i < meshStruct.verticesNumber; ++i)
	{
		meshStruct.vertices[i] = *((float3*)auxVerticesPointer++);
	}

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

	meshStruct.material = 0;
	meshStruct.verticesNumber = parShapeMesh->npoints;
	meshStruct.indicesNumber = parShapeMesh->ntriangles * 3;

	meshStruct.vertices.resize(meshStruct.verticesNumber);
	for (unsigned i = 0u; i < meshStruct.verticesNumber; ++i)
	{
		meshStruct.vertices[i].x = parShapeMesh->points[i * 3];
		meshStruct.vertices[i].y = parShapeMesh->points[(i * 3) + 1];
		meshStruct.vertices[i].z = parShapeMesh->points[(i * 3) + 2];
	}

	GenerateVAO(meshStruct);
}

void ModuleModelLoader::GenerateMaterial(Material& materialStruct)
{
	const aiMaterial* src_material = materialStruct.material;

	aiString file;
	aiTextureMapping mapping;
	unsigned uvindex = 0;

	//if (src_material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, &uvindex) == AI_SUCCESS)
	//{
	//	Material auxMaterialStruct = App->textures->LoadMaterialFromFBX(file.data);
	//	materialStruct.texture0 = auxMaterialStruct.texture0;
	//	materialStruct.width = auxMaterialStruct.width;
	//	materialStruct.height = auxMaterialStruct.height;
	//}
}

void ModuleModelLoader::CreateGameObjectsFromNode(const aiScene* scene, const aiNode* node, GameObject* gameObjectParent)
{
	if (node->mNumChildren > 0)
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{

			if (node->mChildren[i]->mMeshes != nullptr)
			{
				GameObject* gameObjectMesh = App->scene->CreateGameObject(node->mChildren[i]->mName.C_Str(), gameObjectParent, false);
				CreateMeshComponent(scene, node->mChildren[i], gameObjectMesh);
			}
			else
			{
				GameObject* gameObject = App->scene->CreateGameObject(node->mChildren[i]->mName.C_Str(), gameObjectParent, true);

				//TODO: change this to not use recursivity
				CreateGameObjectsFromNode(scene, node->mChildren[i], gameObject);
			}
		}
	}
}

void ModuleModelLoader::CreateMeshComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh)
{
	CreateTransformationComponent(node, gameObjectMesh);

	Mesh meshStruct;
	aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
	GenerateMesh(meshStruct, mesh);

	gameObjectMesh->components.push_back(App->renderer->CreateComponentMesh(gameObjectMesh, ComponentType::MESH, meshStruct));

	if (scene->mMaterials[mesh->mMaterialIndex] != nullptr)
	{
		CreateMaterialComponent(scene, node, gameObjectMesh, mesh->mMaterialIndex);
	}
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
	//materialStruct.color = color;

	gameObjectMesh->components.push_back(App->textures->CreateComponentMaterial(gameObjectMesh, ComponentType::MATERIAL, materialStruct));
}

void ModuleModelLoader::CreateMaterialComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh, unsigned materialIndex)
{
	Material materialStruct;
	//materialStruct.material = scene->mMaterials[materialIndex];
	GenerateMaterial(materialStruct);

	gameObjectMesh->components.push_back(App->textures->CreateComponentMaterial(gameObjectMesh, ComponentType::MATERIAL, materialStruct));
}

void ModuleModelLoader::CreateTransformationComponent(GameObject * gameObject)
{
	float3 position = { 0.0f, 0.0f, 0.0f };
	float3 scale = { 1.0f, 1.0f, 1.0f };
	Quat quatRotation = Quat(0.0f, 0.0f, 0.0f, 1.0f);

	gameObject->components.push_back(new ComponentTransformation(gameObject, ComponentType::TRANSFORMATION, position, scale, quatRotation));
}

void ModuleModelLoader::CreateTransformationComponent(const aiNode* node, GameObject* gameObject)
{
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 position = { translation.x, translation.y, translation.z };
	float3 scale = { scaling.x, scaling.y, scaling.z };
	Quat quatRotation = Quat(rotation.x, rotation.y, rotation.z, rotation.w);

	gameObject->components.push_back(new ComponentTransformation(gameObject, ComponentType::TRANSFORMATION, position, scale, quatRotation));
}

void ModuleModelLoader::GenerateVAO(Mesh & mesh)
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


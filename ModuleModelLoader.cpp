#include "Globals.h"

#include "Application.h"
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

void ModuleModelLoader::Load(const char* filePath, GameObject* gameObjectParent)
{
	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene)
	{
		gameObjectParent->name = std::string(scene->mRootNode->mName.C_Str());

		CreateTransformationComponent(scene->mRootNode, gameObjectParent);

		CreateGameObjectsFromNode(scene, scene->mRootNode, gameObjectParent);

		gameObjectParent->UpdateBoundingBox();

		aiReleaseImport(scene);
	}
	else
	{
		LOG(aiGetErrorString());
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

void ModuleModelLoader::GenerateMesh(Mesh& meshStruct)
{
	const aiMesh* mesh = meshStruct.mesh;

	glGenBuffers(1, &meshStruct.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, meshStruct.vbo);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 + sizeof(float) * 2)*mesh->mNumVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices);

	math::float2* texture_coords = (math::float2*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices,
		sizeof(float) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
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
}

void ModuleModelLoader::GenerateMaterial(Material& materialStruct)
{
	const aiMaterial* src_material = materialStruct.material;

	aiString file;
	aiTextureMapping mapping;
	unsigned uvindex = 0;

	if (src_material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, &uvindex) == AI_SUCCESS)
	{
		Material auxMaterialStruct = App->textures->Load(file.data);
		materialStruct.texture0 = auxMaterialStruct.texture0;
		materialStruct.width = auxMaterialStruct.width;
		materialStruct.height = auxMaterialStruct.height;
	}
}

void ModuleModelLoader::CreateGameObjectsFromNode(const aiScene* scene, const aiNode* node, GameObject* gameObjectParent)
{
	if (node->mNumChildren > 0)
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			GameObject* gameObjectMesh = App->scene->CreateGameObject(node->mChildren[i]->mName.C_Str(), gameObjectParent, false);

			if (node->mChildren[i]->mMeshes != nullptr)
			{
				CreateMeshComponent(scene, node->mChildren[i], gameObjectMesh);
			}
			else
			{
				//TODO: change this to not use recursivity
				CreateGameObjectsFromNode(scene, node->mChildren[i], gameObjectParent);
			}
			gameObjectParent->childrens.push_back(gameObjectMesh);
		}
	}
}

void ModuleModelLoader::CreateMeshComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh)
{
	CreateTransformationComponent(node, gameObjectMesh);

	Mesh meshStruct;
	meshStruct.mesh = scene->mMeshes[node->mMeshes[0]];
	GenerateMesh(meshStruct);

	gameObjectMesh->components.push_back(new ComponentMesh(gameObjectMesh, ComponentType::MESH, meshStruct));
	gameObjectMesh->UpdateBoundingBox();
	if (scene->mMaterials[meshStruct.mesh->mMaterialIndex] != nullptr)
	{
		CreateMaterialComponent(scene, node, gameObjectMesh, meshStruct.mesh->mMaterialIndex);
	}
}

void ModuleModelLoader::CreateMaterialComponent(const aiScene* scene, const aiNode* node, GameObject* gameObjectMesh, unsigned materialIndex)
{
	Material materialStruct;
	materialStruct.material = scene->mMaterials[materialIndex];
	GenerateMaterial(materialStruct);

	gameObjectMesh->components.push_back(new ComponentMaterial(gameObjectMesh, ComponentType::MATERIAL, materialStruct));
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

void ModuleModelLoader::DrawProperties()
{
	GameObject* gameObjectSelected = App->scene->gameObjectSelected;

	if (toggleModelProperties)
	{
		ImGui::Begin("Model", &toggleModelProperties);

		if (gameObjectSelected != nullptr)
		{
			ImGui::Text("Name: %s", gameObjectSelected->name.c_str());
			
			if (gameObjectSelected->components.size() > 0)
			{
				gameObjectSelected->DrawProperties();

				for (auto &component : gameObjectSelected->components)
				{
					component->DrawProperties();
				}
			}
		}

		ImGui::End();
	}
}


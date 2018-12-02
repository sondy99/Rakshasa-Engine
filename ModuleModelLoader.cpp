#include "Globals.h"

#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include "GL/glew.h"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

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

		CreateGameObjectsFromNode(scene, gameObjectParent);

		aiReleaseImport(scene);
	}
	else
	{
		LOG(aiGetErrorString());
	}

}

bool ModuleModelLoader::CleanUp()
{
	for (unsigned i = 0; i < meshes.size(); ++i)
	{
		if (meshes[i].vbo != 0)
		{
			glDeleteBuffers(1, &meshes[i].vbo);
		}

		if (meshes[i].ibo != 0)
		{
			glDeleteBuffers(1, &meshes[i].ibo);
		}
	}

	meshes.clear();

	for (unsigned i = 0; i < materials.size(); ++i)
	{
		if (materials[i].texture0 != 0)
		{
			App->textures->Unload(materials[i].texture0);
		}
	}

	materials.clear();

	return true;
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

	meshes.push_back(meshStruct);
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

		materials.push_back(materialStruct);
	}
}

void ModuleModelLoader::CreateGameObjectsFromNode(const aiScene* scene, GameObject* gameObjectParent)
{
	aiNode* node = scene->mRootNode;

	if (node->mNumChildren > 0)
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			GameObject* gameObjectMesh = App->scene->CreateGameObject(node->mChildren[i]->mName.C_Str(), gameObjectParent);

			Mesh meshStruct;
			meshStruct.mesh = scene->mMeshes[node->mChildren[i]->mMeshes[0]];
			GenerateMesh(meshStruct);

			gameObjectMesh->components.push_back(new ComponentMesh(gameObjectMesh, ComponentType::MESH, meshStruct));
			
			Material materialStruct;
			materialStruct.material = scene->mMaterials[meshStruct.mesh->mMaterialIndex];
			GenerateMaterial(materialStruct);

			gameObjectMesh->components.push_back(new ComponentMaterial(gameObjectMesh, ComponentType::MATERIAL, materialStruct));

			gameObjectParent->childrens.push_back(gameObjectMesh);
		}
	}
}

void ModuleModelLoader::DrawProperties()
{
	if (toggleModelProperties)
	{
		ImGui::Begin("Model", &toggleModelProperties);
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto &texture : materials)
			{
				ImGui::Text("Size:  Width: %d | Height: %d", texture.width, texture.height);
				float size = ImGui::GetWindowWidth();
				ImGui::Image((ImTextureID)texture.texture0, { size, size });
			}
		}
		ImGui::End();
	}
}
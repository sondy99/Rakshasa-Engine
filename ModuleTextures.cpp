#include "Globals.h"
#include "Application.h"

#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "Component.h"
#include "ComponentMaterial.h"

#include "SDL/include/SDL.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"	

ModuleTextures::ModuleTextures()
{
}

ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init()
{
	ilInit();
	iluInit();
	ilutInit();

	return true;
}

bool ModuleTextures::CleanUp()
{
	CleanUpFromList(nullptr);
	return true;
}

std::list<ComponentMaterial*>::iterator ModuleTextures::CleanUpIterator(const std::list<ComponentMaterial*>::iterator iterator)
{
	Unload((*iterator)->material.diffuseMap);
	Unload((*iterator)->material.occlusionMap);
	Unload((*iterator)->material.specularMap);
	Unload((*iterator)->material.emissiveMap);

	RELEASE(*iterator);
	return materials.erase(iterator);
}

void ModuleTextures::CleanUpFromList(const ComponentMaterial * componentMaterial)
{
	for (std::list<ComponentMaterial*>::iterator iterator = materials.begin(); iterator != materials.end();)
	{
		if (componentMaterial == nullptr)
		{
			iterator = CleanUpIterator(iterator);
		}
		else
		{
			if (componentMaterial == *iterator)
			{
				iterator = CleanUpIterator(iterator);
			}
			else
			{
				++iterator;
			}
		}
	}
}

void ModuleTextures::LoadMaterial(const char* path, unsigned& textureID, int& width, int& height)
{
	unsigned imageID;

	ilGenImages(1, &imageID);

	ilBindImage(imageID);

	std::string pathAux(path);

	pathAux.insert(0, "/Library/Textures/");

	LOG("Loading texture %s", pathAux.c_str());

	char* fileBuffer = nullptr;
	unsigned lenghBuffer = App->fileSystem->Load(pathAux.c_str(), &fileBuffer);

	if (ilLoadL(IL_DDS, fileBuffer, lenghBuffer))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
		{
			LOG("Image conversion failed - IL reports error: ");
			LOG(iluErrorString(ilGetError()));
			exit(-1);
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT),
			width, height,
			0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData());

		ilDeleteImages(1, &imageID);
		RELEASE_ARRAY(fileBuffer);
	}

	LOG("Texture creation successful.");
}

void ModuleTextures::LoadMaterial(const char* path, ComponentMaterial* componentMaterial, MaterialTypeSelected materialTypeSelected)
{
	switch (materialTypeSelected)
	{
	case MaterialTypeSelected::OCCLUSION_MAP:
		if (componentMaterial->material.occlusionMap != 0u)
		{
			Unload(componentMaterial->material.occlusionMap);
		}

		LoadMaterial(path, componentMaterial->material.occlusionMap,
			componentMaterial->material.ambientWidth,
			componentMaterial->material.ambientHeight);


		break;
	case MaterialTypeSelected::DIFFUSE_MAP:
		if (componentMaterial->material.diffuseMap != 0u)
		{
			Unload(componentMaterial->material.diffuseMap);
		}

		LoadMaterial(path, componentMaterial->material.diffuseMap,
			componentMaterial->material.diffuseWidth,
			componentMaterial->material.diffuseHeight);


		break;
	case MaterialTypeSelected::SPECULAR_MAP:
		if (componentMaterial->material.specularMap != 0u)
		{
			Unload(componentMaterial->material.specularMap);
		}

		LoadMaterial(path, componentMaterial->material.specularMap,
			componentMaterial->material.specularWidth,
			componentMaterial->material.specularHeight);

		break;
	case MaterialTypeSelected::EMISSIVE_MAP:
		if (componentMaterial->material.emissiveMap != 0u)
		{
			Unload(componentMaterial->material.emissiveMap);
		}

		LoadMaterial(path, componentMaterial->material.emissiveMap,
			componentMaterial->material.emissiveWidth,
			componentMaterial->material.emissiveHeight);

		break;
	}
}

void ModuleTextures::Unload(unsigned id)
{
	if (id != 0u)
	{
		glDeleteTextures(1, &id);
	}
}

ComponentMaterial * ModuleTextures::CreateComponentMaterial()
{
	ComponentMaterial* result = new ComponentMaterial();

	materials.push_back(result);

	return result;
}

ComponentMaterial* ModuleTextures::CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType)
{
	ComponentMaterial* result = new ComponentMaterial(gameObjectParent, componentType);

	materials.push_back(result);

	return result;
}

ComponentMaterial* ModuleTextures::CreateComponentMaterial(GameObject* gameObjectParent, ComponentType componentType, Material material)
{
	ComponentMaterial* result = new ComponentMaterial(gameObjectParent, componentType, material);

	materials.push_back(result);

	return result;
}

void ModuleTextures::RemoveMaterialComponent(const Component* componentToBeRemove)
{
	if (componentToBeRemove->componentType == ComponentType::MATERIAL)
	{
		CleanUpFromList((ComponentMaterial*)componentToBeRemove);
	}
}

void ModuleTextures::RemoveMaterial(ComponentMaterial* componentMaterial)
{
	if (componentMaterial->componentType == ComponentType::MATERIAL)
	{
		Unload(componentMaterial->material.diffuseMap);
		Unload(componentMaterial->material.specularMap);
		Unload(componentMaterial->material.occlusionMap);
		Unload(componentMaterial->material.emissiveMap);

		Material emptyMaterial;

		componentMaterial->material = emptyMaterial;
	}
}

#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
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

Material const ModuleTextures::Load(const char* path)
{
	unsigned imageID;

	GLuint textureID = 0;
	int width = 0;
	int height = 0;

	ilGenImages(1, &imageID); 		

	ilBindImage(imageID); 			

	if (ilLoadImage(path)) {

		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT) {
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
	}
	else
	{
		LOG("Image load failed - IL reports error: ");
		LOG(iluErrorString(ilGetError()));
		exit(-1);
	}

	ilDeleteImages(1, &imageID); 
	LOG("Texture creation successful.");

	return Material(textureID, width, height);
}


void ModuleTextures::Unload(unsigned id)
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}
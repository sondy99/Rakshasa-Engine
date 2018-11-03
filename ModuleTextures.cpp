#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "SDL/include/SDL.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"	

using namespace std;

ModuleTextures::ModuleTextures()
{
}

// Destructor
ModuleTextures::~ModuleTextures()
{
}

// Called before render is available
bool ModuleTextures::Init()
{
	ilInit();
	iluInit();
	ilutInit();

	return true;
}

// Called before quitting
bool ModuleTextures::CleanUp()
{
	return true;
}

// Load new texture from file path
GLuint const ModuleTextures::Load(const char* path)
{
	unsigned imageID;

	GLuint textureID;

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
			//error = ilGetError();
			//std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		//error = ilGetError();
		//std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		exit(-1);
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	//std::cout << "Texture creation successful." << std::endl;

	return textureID; // Return the GLuint to the texture so you can use it!
}


void ModuleTextures::Unload(unsigned id)
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}


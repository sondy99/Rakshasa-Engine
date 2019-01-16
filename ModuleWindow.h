#ifndef __MODULEWINDOW_H__
#define __MODULEWINDOW_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:
	ModuleWindow();
	~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
public:
	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;

	int height = 0;
	int width = 0;
};

#endif __MODULEWINDOW_H__
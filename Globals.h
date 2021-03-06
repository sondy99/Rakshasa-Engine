#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <windows.h>
#include <stdio.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

#define myDegreesToRadians(x) x*(3.141592f/180.0f)

#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 640
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Rakshasa Engine"
#define LOGSSIZE 100

#endif __GLOBAL_H_
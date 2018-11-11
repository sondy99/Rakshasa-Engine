#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

#define degreesToRadians(x) x*(3.141592f/180.0f)

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 640
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Rakshasa Engine"

#endif __GLOBAL_H_
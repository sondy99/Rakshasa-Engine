#ifndef __MODULEINPUT_H__
#define __MODULEINPUT_H__

#include "Module.h"
#include "Point.h"
#include "SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	ModuleInput();
	~ModuleInput();

	update_status PreUpdate() override;
	bool CleanUp() override;

	inline KeyState GetKey(int id) const { return keyboard[id]; }
	inline KeyState GetMouseButtonDown(int id) const { return mouse_buttons[id - 1]; };
	inline bool GetWindowEvent(EventWindow code) const { return windowEvents[code]; };
	inline const iPoint& GetMouseMotion() const { return mouse_motion; };
	inline const iPoint& GetMousePosition() const { return mouse; };
private:
	void HandleDropedFiles(const char* path);
private:
	bool windowEvents[WE_COUNT];
	KeyState* keyboard = nullptr;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	iPoint mouse_motion = { 0, 0 };
	iPoint mouse = { 0, 0 };
};

#endif // __MODULEINPUT_H__
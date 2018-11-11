#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
private:
	void WindowManager();
	void ShowAboutMenu();
private:
	bool toggleAboutMenu = false;
	bool toggleModelProperties = false;
	bool toggleRenderProperties = false;
	bool toggleConsole = false;
	bool toggleWindow = false;
	bool toggleCameraProperties = false;
};

#endif __ModuleEditor_h__

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

	void LogIntoConsole(const char* message);
	void InitImGuiFrame();
	void EndImGuiFrame();
private:
	void WindowManager();
	void DrawAboutMenu();
	void DrawConsole();
	void CreateDockSpace() const;
private:
	bool toggleAboutMenu = false;
	bool toggleModelProperties = true;
	bool toggleRenderProperties = false;
	bool toggleConsole = true;
	bool toggleWindow = false;
	bool toggleCameraProperties = true;	
	ImGuiTextBuffer textBuffer;     
	bool scrollDown;
	bool exit = false;
};

#endif __ModuleEditor_h__

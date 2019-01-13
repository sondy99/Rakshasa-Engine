#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"

#include <vector>

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
	void DrawMemoryViewPort();
	void AddMemory(float memoryPoints);
private:
	bool toggleMemoryViewPort = true;
	bool toggleAboutMenu = false;
	bool toggleConsole = true;
	bool toggleGameObjectProperties = true;
	ImGuiTextBuffer textBuffer;     
	bool scrollDown;
	bool exit = false;
	std::vector<float> memoryPoints;
};

#endif __ModuleEditor_h__

#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"

#include <vector>
#include <string>

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
	void DrawHardwareInfo();
	void CreateDockSpace() const;
	void DrawMemoryViewPort();
	void AddMemory(float memoryPoints);
	void InitHardWareValues();
private:
	bool toggleMemoryViewPort = true;
	bool toggleAboutMenu = false;
	bool toggleConsole = true;
	bool toggleGameObjectProperties = true;
	bool toggleHardwareInfo = false;
	ImGuiTextBuffer textBuffer;     
	bool scrollDown;
	bool exit = false;
	std::vector<float> memoryPoints;

	const char* caps;
	int CPUCount = 0;
	int CPUCacheLineSize = 0;
	int systemRAM = 0;
	bool has3DNow = false;
	bool hasAVX = false;
	bool hasAVX2 = false;
	bool hasAltiVec = false;
	bool hasMMX = false;
	bool hasRDTSC = false;
	bool hasSSE = false;
	bool hasSSE2 = false;
	bool hasSSE3 = false;
	bool hasSSE41 = false;
	bool hasSSE42 = false;
};

#endif __MODULEEDITOR_H__

#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleLibrary.h"
#include "ModuleTime.h"

#include "GameObject.h"

#include "ImGuizmo.h"

#include "mmgr/mmgr.h"

ModuleEditor::ModuleEditor() : memoryPoints(LOGSSIZE)
{
	memoryPoints.resize(LOGSSIZE);
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;          

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();

	LogIntoConsole(TITLE);

	InitHardWareValues();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit"))
			{
				return UPDATE_STOP;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {
			ImGui::Checkbox("Game objects hierarchy", &App->scene->toggleSceneProperties);
			ImGui::Checkbox("Game objects properties", &toggleGameObjectProperties);
			ImGui::Checkbox("Render properties", &App->renderer->toggleRenderProperties);
			ImGui::Checkbox("Camera properties", &App->camera->toggleCameraProperties);
			ImGui::Checkbox("Model properties", &App->modelLoader->toggleModelProperties);
			ImGui::Checkbox("Library properties", &App->library->toggleLibraryProperties);
			ImGui::Checkbox("Memory", &toggleMemoryViewPort);
			ImGui::Checkbox("Hardware info", &toggleHardwareInfo);
			ImGui::Checkbox("Timer info", &App->time->toggleTimerInfo);
			ImGui::Checkbox("Console", &toggleConsole);
			
			//ImGui::Checkbox("Window", &toggleWindow);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Checkbox("About", &toggleAboutMenu);
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	WindowManager();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return update_status();
}

bool ModuleEditor::CleanUp()
{
	exit = true;
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
	return true;
}

void ModuleEditor::WindowManager()
{
	DrawAboutMenu();
	App->renderer->DrawProperties();
	App->camera->DrawProperties();
	App->modelLoader->DrawProperties();
	App->scene->DrawProperties();
	App->library->DrawProperties();
	App->time->DrawProperties();

	if (toggleGameObjectProperties)
	{
		ImGui::Begin("Game object", &toggleGameObjectProperties);
		if (App->scene->GetGameObjectSelected() != nullptr)
		{
			App->scene->GetGameObjectSelected()->DrawProperties();
		}
		ImGui::End();
	}

	DrawConsole();
	DrawMemoryViewPort();
	DrawHardwareInfo();

	App->renderer->DrawCameraSceneWindow();
	App->renderer->DrawCameraGameWindow();
}

void ModuleEditor::InitImGuiFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	CreateDockSpace();
}

void ModuleEditor::EndImGuiFrame()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::CreateDockSpace() const
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ (float)App->window->width, (float)App->window->height });
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", NULL, windowFlags);
	ImGui::PopStyleVar(3);

	ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
}

void ModuleEditor::DrawConsole()
{
	if (toggleConsole)
	{
		ImGui::Begin("Console", &toggleConsole);
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::TextUnformatted(textBuffer.begin());

		if (scrollDown)
			ImGui::SetScrollHereY(1.0f);

		scrollDown = false;
		ImGui::EndChild();
		ImGui::End();
	}
}

void ModuleEditor::DrawHardwareInfo()
{
	if (toggleHardwareInfo)
	{
		ImGui::Begin("HardwareInfo", &toggleHardwareInfo);

		ImGui::Text("CPUs:");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
		
		ImGui::Text("%d (Cache: %dB)", CPUCount, CPUCacheLineSize);
		ImGui::PopStyleColor();

		ImGui::Text("System RAM:");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
		ImGui::Text("%dGb", systemRAM);
		ImGui::PopStyleColor();

		ImGui::Text("Caps:");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 1,1,0,1 });
		
		ImGui::Text(caps);
		ImGui::PopStyleColor();

		ImGui::End();
	}
}

void ModuleEditor::DrawMemoryViewPort()
{
	if (toggleMemoryViewPort)
	{
		ImGui::Begin("Memory", &toggleMemoryViewPort);

		sMStats stats = m_getMemoryStatistics();
		AddMemory((float)stats.totalReportedMemory);

		ImGui::PlotHistogram("##memory", &memoryPoints[0], memoryPoints.size(), 0, "Memory Consumption (Bytes)", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(0, 80));

		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);

		ImGui::End();
	}
}

void ModuleEditor::DrawAboutMenu()
{
	if (toggleAboutMenu)
	{
		ImGui::Begin("About", &toggleAboutMenu);

		if (ImGui::MenuItem(TITLE))
		{
			ShellExecute(0, 0, "https://github.com/sondy99/Rakshasa-Engine", 0, 0, SW_SHOW);
		}

		ImGui::Text("Authors:");
		ImGui::Text("Daniel Cardoza");

		ImGui::Separator();

		ImGui::Text("Libraries:");
		if (ImGui::MenuItem("SDL v2.0.8"))
		{
			ShellExecute(0, 0, "https://www.libsdl.org/index.php", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("Glew v2.1.0"))
		{
			ShellExecute(0, 0, "http://glew.sourceforge.net/", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("ImGui v1.66"))
		{
			ShellExecute(0, 0, "https://github.com/ocornut/imgui/tree/docking", 0, 0, SW_SHOW);
		}
		if (ImGui::MenuItem("Devil v1.8.0"))
		{
			ShellExecute(0, 0, "http://openil.sourceforge.net/", 0, 0, SW_SHOW);
		}

		ImGui::End();
	}
}

void ModuleEditor::LogIntoConsole(const char* message)
{
	if (!exit)
	{
		textBuffer.appendf(message);
		scrollDown = true;
	}
}

void ModuleEditor::AddMemory(float memoryValue) {

	for (unsigned i = 0u; i < LOGSSIZE - 1; ++i) {
		memoryPoints[i] = memoryPoints[i + 1];
	}

	memoryPoints[LOGSSIZE - 1] = memoryValue;
}

void ModuleEditor::InitHardWareValues()
{
	std::string capsAux;
	CPUCount = SDL_GetCPUCount();
	CPUCacheLineSize = SDL_GetCPUCacheLineSize();
	systemRAM = SDL_GetSystemRAM() / 1000;

	has3DNow = SDL_Has3DNow();
	hasAVX = SDL_HasAVX();
	hasAVX2 = SDL_HasAVX2();
	hasAltiVec = SDL_HasAltiVec();
	hasMMX = SDL_HasMMX();
	hasRDTSC = SDL_HasRDTSC();
	hasSSE = SDL_HasSSE();
	hasSSE2 = SDL_HasSSE2();
	hasSSE3 = SDL_HasSSE3();
	hasSSE41 = SDL_HasSSE41();
	hasSSE42 = SDL_HasSSE42();

	if (has3DNow)
		capsAux += "3DNow!, ";
	if (hasAVX)
		capsAux += "AVX, ";
	if (hasAVX2)
		capsAux += "AVX2, ";
	if (hasAltiVec)
		capsAux += "AltiVec, ";
	if (hasMMX)
		capsAux += "MMX, ";
	if (hasRDTSC)
		capsAux += "RDTSC, ";
	if (capsAux.size() > 5)
		capsAux += "\n";
	if (hasSSE)
		capsAux += "SSE, ";
	if (hasSSE2)
		capsAux += "SSE2, ";
	if (hasSSE3)
		capsAux += "SSE3, ";
	if (hasSSE41)
		capsAux += "SSE41, ";
	if (hasSSE42)
		capsAux += "SSE42, ";

	caps = capsAux.c_str();
}

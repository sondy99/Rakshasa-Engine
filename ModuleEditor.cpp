#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"

ModuleEditor::ModuleEditor()
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	LogIntoConsole(TITLE);

	return true;
}
bool showAboutMenu;
update_status ModuleEditor::PreUpdate()
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
			ImGui::Checkbox("Render properties", &toggleRenderProperties);
			ImGui::Checkbox("Camera properties", &toggleCameraProperties);
			ImGui::Checkbox("Model properties", &toggleModelProperties);
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
		
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
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
	if (toggleAboutMenu)
	{
		DrawAboutMenu();
	}
	if(toggleRenderProperties)
	{
		App->renderer->DrawProperties();
	}
	if (toggleCameraProperties)
	{
		App->camera->DrawProperties();
	}
	if (toggleModelProperties)
	{
		App->modelLoader->DrawProperties();
	}
	if (toggleConsole)
	{
		DrawConsole();
	}

	App->renderer->DrawCameraWindow();
}

void ModuleEditor::DrawAboutMenu() 
{
	ImGui::Begin("About");

	if (ImGui::MenuItem(TITLE))
	{
		ShellExecute(0, 0, "https://github.com/sondy99/sondy99/Rakshasa-Engine", 0, 0, SW_SHOW);
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

void ModuleEditor::LogIntoConsole(const char* message)
{
	if (!exit)
	{
		textBuffer.appendf(message);
		scrollDown = true;
	}
}

void ModuleEditor::DrawConsole()
{
	ImGui::Begin("Console");
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextUnformatted(textBuffer.begin());

	if (scrollDown)
		ImGui::SetScrollHereY(1.0f);

	scrollDown = false;
	ImGui::EndChild();
	ImGui::End();
}
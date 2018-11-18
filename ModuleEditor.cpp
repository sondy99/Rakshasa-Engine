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
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();

	LogIntoConsole(TITLE);

	return true;
}

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
			ImGui::Checkbox("Render properties", &App->renderer->toggleRenderProperties);
			ImGui::Checkbox("Camera properties", &App->camera->toggleCameraProperties);
			ImGui::Checkbox("Model properties", &App->modelLoader->toggleModelProperties);
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
	DrawAboutMenu();
	App->renderer->DrawProperties();
	App->camera->DrawProperties();
	App->modelLoader->DrawProperties();
	DrawConsole();

	App->renderer->DrawSceneWindow();
}

void ModuleEditor::InitImGuiFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

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

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", NULL, window_flags);
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

void ModuleEditor::DrawAboutMenu()
{
	if (toggleAboutMenu)
	{
		ImGui::Begin("About", &toggleAboutMenu);

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
}

void ModuleEditor::LogIntoConsole(const char* message)
{
	if (!exit)
	{
		textBuffer.appendf(message);
		scrollDown = true;
	}
}

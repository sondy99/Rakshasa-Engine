#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

ModuleTime::ModuleTime() 
{ 
}

ModuleTime::~ModuleTime() 
{ 
}

bool ModuleTime::Init() 
{
	gameDeltaTime = 0.0f;
	frameTimer.Start();
	fpsTimer.Start();
	return true;
}

update_status ModuleTime::Update() 
{
	++frameCount;
	++realFrameCount;

	realDeltaTime = frameTimer.ReadSeconds(); 	
	realTime += realDeltaTime;

	//App->editor->AddFPSCount(FPS, realDeltaTime * 1000.0f);

	if (gameState == GameState::RUN) 
	{
		//App->editor->AddGameFPSCount(FPS, gameDeltaTime * gameTimeScale * 1000.0f);
		++totalFrames;
		gameDeltaTime = frameTimer.ReadSeconds();	
		gameTime += gameDeltaTime * gameTimeScale;			
	}

	frameTimer.Reset();

	// Frames per second
	if (fpsTimer.ReadSeconds() >= 1.0f) 
	{
		FPS = frameCount;
		frameCount = 0u;
		fpsTimer.Reset();
	}

	return UPDATE_CONTINUE;
}

bool ModuleTime::CleanUp() 
{
	frameTimer.Stop();
	fpsTimer.Stop();
	return true;
}

void ModuleTime::DrawProperties()
{
	if (toggleTimerInfo)
	{
		ImGui::Begin("Time", &toggleTimerInfo, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

		ImVec2 size = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(-(App->window->width - size.x) * 0.5f, -(App->window->height - size.y) * 0.5f));

		ImGui::BeginMenuBar();

		if (gameState == GameState::STOP)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f, 0.5f, 0.5f, 0.7f });
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.3f, 0.5f, 0.3f, 0.7f });
		}

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f, 1.0f, 1.0f, 0.2f });

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 85) / 2);
		if (ImGui::ArrowButton("Play", ImGuiDir_Right))
		{
			if (gameState == GameState::STOP)
			{
				StartGameClock();
				App->scene->SaveTemporaryScene();
			}
			else
			{
				StopGameClock();
				App->scene->LoadTemporaryScene();
			}
		}

		ImGui::PopStyleColor(2);
	
		ImGui::EndMenuBar();
		ImGui::End();
	}
}

void ModuleTime::StartGameClock() 
{
	gameState = GameState::RUN;
}

void ModuleTime::PauseGameClock(bool pause) 
{
	if (pause) 
	{
		gameState = GameState::PAUSE;
		gameDeltaTime = 0.0f;
	} else 
	{
		gameState = GameState::RUN;
	}
}

void ModuleTime::StopGameClock() 
{
	gameState = GameState::STOP;
	gameDeltaTime = 0.0f;
	gameTime = 0.0f;
	totalFrames = 0u;
}

void ModuleTime::Step() 
{
	nextFrame = true;
}

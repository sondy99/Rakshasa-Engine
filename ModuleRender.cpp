#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleEnvironment.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransformation.h"
#include "ComponentCamera.h"

#include "SDL.h"
#include "GL/glew.h"

ModuleRender::ModuleRender()
{
	fpsList.resize(100, 0.0f);
	msList.resize(100, 0.0f);
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

	int width, height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	glViewport(0, 0, width, height);

	App->shader->LoadShaders(App->shader->program, "default.vs", "default.fs");

	frameBufferScene.frameBufferType = FrameBufferType::SCENE;
	InitFrameBuffer(App->window->width, App->window->height, frameBufferScene);
	frameBufferGame.frameBufferType = FrameBufferType::GAME;
	InitFrameBuffer(App->window->width, App->window->height, frameBufferGame);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	BROFILER_CATEGORY("RenderPreUpdate()", Profiler::Color::AliceBlue);

	manageFpsAndMsList();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->editor->InitImGuiFrame();

	CalculateGameObjectGlobalMatrix(App->scene->root);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
	BROFILER_CATEGORY("RenderUpdate()", Profiler::Color::Aqua);

	math::float4x4 viewScene = App->camera->sceneCamera->LookAt(App->camera->sceneCamera->cameraPosition, App->camera->sceneCamera->cameraFront, App->camera->sceneCamera->cameraUp);
	math::float4x4 projectionScene = App->camera->sceneCamera->ProjectionMatrix();

	RenderUsingSpecificFrameBuffer(frameBufferScene, App->camera->sceneCamera, viewScene, projectionScene);

	if (componentCameraGameSelected != nullptr)
	{
		math::float4x4 viewGame = componentCameraGameSelected->LookAt(componentCameraGameSelected->cameraPosition,
			componentCameraGameSelected->cameraFront, componentCameraGameSelected->cameraUp);
		math::float4x4 projectionGame = componentCameraGameSelected->ProjectionMatrix();

		RenderUsingSpecificFrameBuffer(frameBufferGame, componentCameraGameSelected, viewGame, projectionGame);
	}

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("RenderPostUpdate()", Profiler::Color::Orchid);

	App->editor->EndImGuiFrame();

	SDL_GL_SwapWindow(App->window->window);

	FpsCount();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	glDeleteFramebuffers(1, &frameBufferScene.frameBufferObject);
	glDeleteRenderbuffers(1, &frameBufferScene.renderBufferObject);
	glDeleteFramebuffers(1, &frameBufferGame.frameBufferObject);
	glDeleteRenderbuffers(1, &frameBufferGame.renderBufferObject);

	return true;
}

void ModuleRender::RenderMesh(const Mesh& mesh, const Material& material,
	unsigned program, const math::float4x4& model,
	const math::float4x4& view, const math::float4x4& proj)
{
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.texture0);
	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * mesh.verticesNumber));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	glDrawElements(GL_TRIANGLES, mesh.indicesNumber, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ModuleRender::DrawProperties()
{
	if (toggleRenderProperties)
	{
		ImGui::Begin("Render properties", &toggleRenderProperties);
		if (minFps != 0.0f && minFps != 100.0f)
		{
			/*char minFpsMessage[35];
			sprintf_s(minFpsMessage, 35, "Slowest frame rate per second %0.1f", minFps);
			ImGui::Text(minFpsMessage);

			char message[20];
			sprintf_s(message, 20, "Framerate %0.1f", fpsList[fpsList.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &fpsList[0], fpsList.size(), 0, message, 0.0f, 200.0f, ImVec2(310, 100));

			ImGui::Separator();

			char minMsMessage[40];
			sprintf_s(minMsMessage, 40, "Max millisecod per second %0.1f", maxMs);
			ImGui::Text(minMsMessage);

			sprintf_s(message, 20, "Milliseconds %0.1f", msList[msList.size() - 1]);
			ImGui::PlotHistogram("##Milliseconds", &msList[0], msList.size(), 0, message, 0.0f, 40.0f, ImVec2(310, 100));*/
		}

		ImGui::End();
	}
}

void ModuleRender::DrawCameraSceneWindow()
{
	ImGui::Begin("Scene", &sceneEnabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::IsWindowFocused())
	{
		App->camera->selectedCamera = App->camera->sceneCamera;
		App->camera->viewPortIsFocused = ImGui::IsWindowHovered();
	}

	ImVec2 size = ImGui::GetWindowSize();

	App->camera->SetScreenNewScreenSize(size.x, size.y);

	ImGui::Image((ImTextureID)frameBufferScene.renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });

	ImGui::End();
}

void ModuleRender::DrawCameraGameWindow()
{
	ImGui::Begin("Game", &sceneEnabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoFocusOnAppearing);

	std::list<GameObject*> camerasGameObject = App->scene->GetGameCameras();

	manageComboBoxCamera(camerasGameObject);
	if (camerasGameObject.size() > 0)
	{
		if (componentCameraGameSelected)
		{
			if (ImGui::IsWindowFocused())
			{
				App->camera->selectedCamera = componentCameraGameSelected;
				App->camera->viewPortIsFocused = ImGui::IsWindowHovered();
			}

			ImVec2 size = ImGui::GetWindowSize();

			componentCameraGameSelected->SetScreenNewScreenSize(size.x, size.y);

			ImGui::Image((ImTextureID)frameBufferGame.renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });
		}
	}

	ImGui::End();
}

void ModuleRender::manageComboBoxCamera(std::list<GameObject*> camerasGameObject)
{
	static const char* labelCurrentCameraGameObjecteName = "Select a camera";
	
	if (camerasGameObject.size() > 0)
	{
		if (ImGui::BeginCombo("##combo", labelCurrentCameraGameObjecteName))
		{
			for (std::list<GameObject*>::iterator iterator = camerasGameObject.begin(); iterator != camerasGameObject.end(); ++iterator)
			{
				bool isSelected = (labelCurrentCameraGameObjecteName == (*iterator)->name.c_str());
				if (ImGui::Selectable((*iterator)->name.c_str(), isSelected))
				{
					labelCurrentCameraGameObjecteName = (*iterator)->name.c_str();
					componentCameraGameSelected = (ComponentCamera*)(*iterator)->GetComponent(ComponentType::CAMERA);
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	else
	{
		labelCurrentCameraGameObjecteName = "Select a camera";
	}
}

void ModuleRender::RenderBoundingBox(GameObject * gameObject, FrameBufferType frameBufferType)
{
	if (gameObject->isSelected && frameBufferType == FrameBufferType::SCENE)
	{
		App->environment->DrawBoundingBox(gameObject);
	}
}

void ModuleRender::FpsCount()
{
	++frameCounter;
	ticksNow = SDL_GetTicks();
	deltaTime = (float)(ticksNow - lastTickTime) * 0.001;
	lastTickTime = ticksNow;
	auxTimer += deltaTime;
	if (auxTimer >= 1.0f) {
		fps = frameCounter;
		auxTimer = 0;
		frameCounter = 0;
	}
}

void ModuleRender::manageFpsAndMsList()
{
	fpsList.erase(fpsList.begin());
	fpsList.push_back(fps);

	float ms = deltaTime * 1000;
	msList.erase(msList.begin());
	msList.push_back(ms);

	if (ticksNow > 5000)
	{
		minFps = (minFps > fps) ? fps : minFps;
		maxMs = (maxMs < ms) ? ms : maxMs;
	}

}

void ModuleRender::InitFrameBuffer(int width, int height, FrameBufferStruct &frameBufferToInit)
{
	glDeleteFramebuffers(1, &frameBufferToInit.frameBufferObject);
	glDeleteRenderbuffers(1, &frameBufferToInit.renderBufferObject);

	glGenFramebuffers(1, &frameBufferToInit.frameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferToInit.frameBufferObject);

	glGenTextures(1, &frameBufferToInit.renderTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferToInit.renderTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferToInit.renderTexture, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &frameBufferToInit.renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBufferToInit.renderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBufferToInit.renderBufferObject);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::RenderComponentFromGameObject(GameObject * gameObject, math::float4x4 view, math::float4x4 projection, FrameBufferType frameBufferType)
{
	for (auto &gameObjectChild : gameObject->childrens)
	{
		if (gameObjectChild->active)			
		{
			if (gameObjectChild->childrens.size() > 0)
			{
				ComponentCamera* camera = (ComponentCamera*)gameObjectChild->GetComponent(ComponentType::CAMERA);

				if (camera == nullptr)
				{
					//TODO: change this to not use recursivity
					RenderComponentFromGameObject(gameObjectChild, view, projection, frameBufferType);
				}
			}

			ComponentTransformation* transformation = (ComponentTransformation*)gameObjectChild->GetComponent(ComponentType::TRANSFORMATION);
			ComponentMaterial* componentMaterial = (ComponentMaterial*)gameObjectChild->GetComponent(ComponentType::MATERIAL);
			ComponentMesh* componentMesh = (ComponentMesh*)gameObjectChild->GetComponent(ComponentType::MESH);

			if ((componentMaterial != nullptr && componentMesh != nullptr) &&
				(componentCameraGameSelected == nullptr || componentCameraGameSelected->frustum.Intersects(gameObjectChild->boundingBox)))
			{
				RenderMesh(componentMesh->mesh, componentMaterial->material, App->shader->program,
					transformation->globalModelMatrix, view, projection);

				RenderBoundingBox(gameObjectChild, frameBufferType);
			}
		}
	}
	RenderBoundingBox(gameObject, frameBufferType);
}

void ModuleRender::CalculateGameObjectGlobalMatrix(GameObject* gameObject)
{
	ComponentTransformation* transformation = (ComponentTransformation*)gameObject->GetComponent(ComponentType::TRANSFORMATION);

	if (transformation != nullptr)
	{
		if (gameObject->parent == App->scene->root || gameObject->parent == nullptr)
		{
			transformation->globalModelMatrix = transformation->localModelMatrix;
		}
		else
		{
			ComponentTransformation* transformationParent = (ComponentTransformation*)gameObject->parent->GetComponent(ComponentType::TRANSFORMATION);
			transformation->globalModelMatrix = transformationParent->globalModelMatrix*transformation->localModelMatrix;
		}

		gameObject->UpdateBoundingBoxTransformation();
	}

	for (auto &gameObjectChild : gameObject->childrens)
	{
		//TODO: change this to not use recursivity
		CalculateGameObjectGlobalMatrix(gameObjectChild);
	}
}

void ModuleRender::RenderUsingSpecificFrameBuffer(FrameBufferStruct frameBufferToRender, ComponentCamera* camera, math::float4x4 view, math::float4x4 projection)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferToRender.frameBufferObject);

	App->environment->DrawReferenceGround();
	App->environment->DrawReferenceAxis();

	if (frameBufferToRender.frameBufferType == FrameBufferType::SCENE && componentCameraGameSelected != nullptr)
	{
		App->environment->DrawFrustum(componentCameraGameSelected);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	App->debugDraw->Draw(frameBufferToRender.frameBufferObject, camera->screenWidth, camera->screenHeight, view, projection);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferToRender.frameBufferObject);

	RenderComponentFromGameObject(App->scene->root, view, projection, frameBufferToRender.frameBufferType);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->debugDraw->Draw(frameBufferToRender.frameBufferObject, camera->screenWidth, camera->screenHeight, view, projection);
}


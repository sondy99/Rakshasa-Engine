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
#include "ModuleCamera.h"

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
	glEnable(GL_BLEND);

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

	ManageFpsAndMsList();

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

	for (std::list<ComponentMesh*>::iterator iterator = meshes.begin(); iterator != meshes.end();)
	{
		RELEASE(*iterator);
		iterator = meshes.erase(iterator);
	}

	RELEASE(componentCameraGameSelected);

	return true;
}

void ModuleRender::RenderMesh(const ComponentMesh& componentMesh, const ComponentMaterial* componentMaterial,
	const math::float4x4& model, const math::float4x4& view, const math::float4x4& proj)
{
	Mesh mesh = componentMesh.mesh;

	if (componentMesh.isWireframeActive)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	unsigned program = App->shader->program;
	
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	if (componentMaterial != nullptr)
	{
		glUniform4f(glGetUniformLocation(program, "diffuseColor"), componentMaterial->material.diffuseColor.x, componentMaterial->material.diffuseColor.y, componentMaterial->material.diffuseColor.z, 1.0f);
		glUniform4f(glGetUniformLocation(program, "emissiveColor"), componentMaterial->material.emissiveColor.x, componentMaterial->material.emissiveColor.y, componentMaterial->material.emissiveColor.z, 1.0f);
		glUniform4f(glGetUniformLocation(program, "specularColor"), componentMaterial->material.specularColor.x, componentMaterial->material.specularColor.y, componentMaterial->material.specularColor.z, 1.0f);


		glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, &App->scene->ambientLightPosition[0]);
		glUniform1f(glGetUniformLocation(program, "ambient"), App->scene->ambientLight);
		glUniform1f(glGetUniformLocation(program, "shininess"), componentMaterial->material.shininess);
		glUniform1f(glGetUniformLocation(program, "k_ambient"), componentMaterial->material.ambientK);
		glUniform1f(glGetUniformLocation(program, "k_diffuse"), componentMaterial->material.diffuseK);
		glUniform1f(glGetUniformLocation(program, "k_specular"), componentMaterial->material.specularK);


		if (componentMaterial->material.diffuseMap != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, componentMaterial->material.diffuseMap);
			glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);
			glUniform1i(glGetUniformLocation(program, "useDiffuseMap"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "useDiffuseMap"), 0);
		}

		if (componentMaterial->material.emissiveMap != 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, componentMaterial->material.emissiveMap);
			glUniform1i(glGetUniformLocation(program, "emissiveMap"), 1);
			glUniform1i(glGetUniformLocation(program, "useEmissiveMap"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "useEmissiveMap"), 0);
		}

		if (componentMaterial->material.occlusionMap != 0)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, componentMaterial->material.occlusionMap);
			glUniform1i(glGetUniformLocation(program, "occlusionMap"), 2);
			glUniform1i(glGetUniformLocation(program, "useOcclusionMap"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "useOcclusionMap"), 0);
		}

		if (componentMaterial->material.specularMap != 0)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, componentMaterial->material.specularMap);
			glUniform1i(glGetUniformLocation(program, "specularMap"), 3);
			glUniform1i(glGetUniformLocation(program, "useSpecularMap"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "useSpecularMap"), 0);
		}
	}

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indicesNumber, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

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
			char minFpsMessage[40];
			sprintf_s(minFpsMessage, 40, "Slowest frame rate per second %0.1f", minFps);
			ImGui::Text(minFpsMessage);

			char message[40];
			sprintf_s(message, 40, "Framerate %0.1f", fpsList[fpsList.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &fpsList[0], fpsList.size(), 0, message, 0.0f, 200.0f, ImVec2(310, 100));

			ImGui::Separator();

			char minMsMessage[40];
			sprintf_s(minMsMessage, 40, "Max millisecod per second %0.1f", maxMs);
			ImGui::Text(minMsMessage);

			sprintf_s(message, 40, "Milliseconds %0.1f", msList[msList.size() - 1]);
			ImGui::PlotHistogram("##Milliseconds", &msList[0], msList.size(), 0, message, 0.0f, 40.0f, ImVec2(310, 100));
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

	std::list<ComponentCamera*> componentCameras = App->camera->cameras;

	ManageComboBoxCamera(componentCameras);
	if (componentCameras.size() > 0)
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
	else
	{
		componentCameraGameSelected = nullptr;
	}

	ImGui::End();
}

ComponentMesh* ModuleRender::CreateComponentMesh()
{
	ComponentMesh* result = new ComponentMesh();
	meshes.push_back(result);

	return result;
}

ComponentMesh* ModuleRender::CreateComponentMesh(GameObject* gameObjectParent, ComponentType componentType)
{
	ComponentMesh* result = new ComponentMesh(gameObjectParent, componentType);
	meshes.push_back(result);

	return result;
}

ComponentMesh* ModuleRender::CreateComponentMesh(GameObject* gameObjectParent, ComponentType componentType, Mesh mesh)
{
	ComponentMesh* result = new ComponentMesh(gameObjectParent, componentType, mesh);
	meshes.push_back(result);

	return result;
}

void ModuleRender::RemoveMeshComponent(Component* componentToBeRemove)
{
	if (componentToBeRemove->componentType == ComponentType::MESH)
	{
		meshes.remove((ComponentMesh*)componentToBeRemove);
	}
}

void ModuleRender::ManageComboBoxCamera(std::list<ComponentCamera*> componentCameras)
{
	static const char* labelCurrentCameraGameObjecteName = "Select a camera";
	
	if (componentCameras.size() > 0)
	{
		if (componentCameraGameSelected != nullptr)
		{
			labelCurrentCameraGameObjecteName = componentCameraGameSelected->gameObjectParent->name.c_str();
		}

		if (ImGui::BeginCombo("##combo", labelCurrentCameraGameObjecteName))
		{
			for (std::list<ComponentCamera*>::iterator iterator = componentCameras.begin(); iterator != componentCameras.end(); ++iterator)
			{
				bool isSelected = (labelCurrentCameraGameObjecteName == (*iterator)->gameObjectParent->name.c_str());
				if (ImGui::Selectable((*iterator)->gameObjectParent->name.c_str(), isSelected))
				{
					labelCurrentCameraGameObjecteName = (*iterator)->gameObjectParent->name.c_str();
					componentCameraGameSelected = (ComponentCamera*)(*iterator);
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

void ModuleRender::ManageFpsAndMsList()
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

void ModuleRender::RenderComponentFromMeshesList(math::float4x4 view, math::float4x4 projection, FrameBufferType frameBufferType)
{
	for (auto &componentMesh : meshes)
	{
		componentMesh->UpdateGlobalBoundingBox();

		if (componentMesh->gameObjectParent->active)
		{
			ComponentTransformation* transformation = (ComponentTransformation*)componentMesh->gameObjectParent->GetComponent(ComponentType::TRANSFORMATION);
			ComponentMaterial* componentMaterial = (ComponentMaterial*)componentMesh->gameObjectParent->GetComponent(ComponentType::MATERIAL);

			if ((componentMesh != nullptr) &&
				(componentCameraGameSelected == nullptr ||
					componentCameraGameSelected->frustum.Intersects(componentMesh->globalBoundingBox) ||
					!App->scene->isSceneCullingActive))
			{
				RenderMesh(*componentMesh, componentMaterial,
					transformation->globalModelMatrix, view, projection);
			}

			if (componentMesh != nullptr && componentMesh->gameObjectParent->isSelected)
			{
				App->environment->DrawBoundingBox(*componentMesh);
			}
		}
	}
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
			transformation->globalModelMatrix = transformationParent->globalModelMatrix * transformation->localModelMatrix;
		}
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
		App->environment->DrawFrustum(*componentCameraGameSelected);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	App->debugDraw->Draw(frameBufferToRender.frameBufferObject, camera->screenWidth, camera->screenHeight, view, projection);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferToRender.frameBufferObject);

	RenderComponentFromMeshesList(view, projection, frameBufferToRender.frameBufferType);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->debugDraw->Draw(frameBufferToRender.frameBufferObject, camera->screenWidth, camera->screenHeight, view, projection);
}


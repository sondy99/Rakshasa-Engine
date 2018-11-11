#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleEnvironment.h"

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

	App->shader->LoadShaders(App->shader->program, "../default.vs", "../default.fs");

	return true;
}

update_status ModuleRender::PreUpdate()
{
	manageFpsAndMsList();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	math::float4x4 projection = App->camera->ProjectionMatrix();
	math::float4x4 view = App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp);
	math::float4x4 model = math::float4x4::identity;

	glUseProgram(App->shader->colorProgram);

	glUniformMatrix4fv(glGetUniformLocation(App->shader->colorProgram, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->colorProgram, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->colorProgram, "proj"), 1, GL_TRUE, &projection[0][0]);

	App->environment->DrawReferenceGround();
	App->environment->DrawReferenceAxis();

	glUseProgram(0);

	for (unsigned i = 0; i < App->modelLoader->meshes.size(); ++i)
	{
		const ModuleModelLoader::Mesh& mesh = App->modelLoader->meshes[i];

		RenderMesh(mesh, App->modelLoader->materials[mesh.material], App->shader->program,
			App->modelLoader->transform, view, projection);
	}
	
	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);

	FpsCount();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
	
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
    glViewport(0, 0, width, height); 
}

void ModuleRender::RenderMesh(const ModuleModelLoader::Mesh& mesh, const ModuleModelLoader::Material& material,
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
	ImGui::Begin("Render properties");
	char minFpsMessage[35];
	sprintf_s(minFpsMessage, 35, "Slowest frame rate per second %0.1f", minFps);
	ImGui::Text(minFpsMessage);

	char message[20];
	sprintf_s(message, 20, "Framerate %0.1f", fpsList[fpsList.size() - 1]);
	ImGui::PlotHistogram("##Framerate", &fpsList[0], fpsList.size(), 0, message, 0.0f, 200.0f, ImVec2(310, 100));
	
	ImGui::Separator();

	char minMsMessage[40];
	sprintf_s(minMsMessage, 40, "Slowest millisecod per second %0.1f", maxMs);
	ImGui::Text(minMsMessage);

	sprintf_s(message, 20, "Milliseconds %0.1f", msList[msList.size() - 1]);
	ImGui::PlotHistogram("##Milliseconds", &msList[0], msList.size(), 0, message, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();
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

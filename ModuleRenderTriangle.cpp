#include "Application.h"

#include "ModuleRenderTriangle.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"
#include "ModuleEnvironment.h"

#include "GL/glew.h"
#include "SDL.h"

ModuleRenderTriangle::ModuleRenderTriangle()
{
}

ModuleRenderTriangle::~ModuleRenderTriangle()
{
}

bool ModuleRenderTriangle::Init()
{
	App->shader->LoadShaders("../default.vs", "../default.fs");

	texture0 = App->textures->Load("Lenna.png");

	float positionsBuffer[] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	unsigned int indices[] =
	{
		0,1,2,
		1,3,2
	};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionsBuffer), positionsBuffer, GL_STATIC_DRAW);

	//Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	//UV position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * 4));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//index buffer
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
    return vbo;
}

update_status ModuleRenderTriangle::Update()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	math::float4x4 projection = App->camera->ProjectionMatrix();
	math::float4x4 view = App->camera->LookAt(App->camera->cameraPos, App->camera->cameraFront, App->camera->cameraUp);
	math::float4x4 model = math::float4x4::identity;

	glUseProgram(App->shader->program);

	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "proj"), 1, GL_TRUE, &projection[0][0]);
	
	//int location = glGetUniformLocation(App->shader->program, "uColor");
	//glUniform4f(location, 1.0f, 0.5f, 0.2f, 0.8f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	glUniform1i(glGetUniformLocation(App->shader->program, "texture0"), 0);

	App->environment->DrawReferenceGround();
	App->environment->DrawReferenceAxis();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

	return UPDATE_CONTINUE;
}

bool ModuleRenderTriangle::CleanUp()
{
    if(vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }

	return true;
}
#include "Application.h"

#include "ModuleRenderTriangle.h"
#include "ModuleWindow.h"
#include "ModuleShader.h"
#include "ModuleCamera.h"

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
	
	float positionsBuffer[] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	unsigned int indices[] =
	{
		0,1,2,
		1,3,2
	};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionsBuffer), positionsBuffer, GL_STATIC_DRAW);

	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return vbo;
}

update_status ModuleRenderTriangle::Update()
{

	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	math::float4x4 projection = App->camera->GetProjectionMatrix();
	math::float4x4 view = App->camera->LookAt(App->camera->position, App->camera->front, App->camera->up);
	math::float4x4 model = math::float4x4::identity;

	glUseProgram(App->shader->program);

	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "proj"), 1, GL_TRUE, &projection[0][0]);

	int location = glGetUniformLocation(App->shader->program, "uColor");
	
	glUniform4f(location, 1.0f, 0.5f, 0.2f, 0.8f);

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
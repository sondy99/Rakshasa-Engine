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
	
	float vertex_buffer_data[] =
	{
        // positions
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

	};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo;
}

update_status ModuleRenderTriangle::Update()
{

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
            0,                  // attribute 0
            3,                  // number of componentes (3 floats)
            GL_FLOAT,           // data type
            GL_FALSE,           // should be normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );


	math::float4x4 projection = App->camera->GetProjectionMatrix();
	math::float4x4 view = App->camera->LookAt(App->camera->position, App->camera->front, App->camera->up);
	math::float4x4 model = math::float4x4::identity;

	glUseProgram(App->shader->program);

	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "view"), 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->shader->program, "proj"), 1, GL_TRUE, &projection[0][0]);

	int location = glGetUniformLocation(App->shader->program, "uColor");
	
	glUniform4f(location, 1.0f, 0.5f, 0.2f, 0.8f);

	glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
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
#include "ModuleEnvironment.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "GL/glew.h"
#include "SDL.h"

ModuleEnvironment::ModuleEnvironment()
{
}


ModuleEnvironment::~ModuleEnvironment()
{
}


bool ModuleEnvironment::Init()
{
	return true;
}

update_status ModuleEnvironment::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEnvironment::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEnvironment::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModuleEnvironment::CleanUp()
{
	return true;
}


void ModuleEnvironment::DrawReferenceGround() {
	glLineWidth(1.0f);
	float d = 200.0f;

	/*int fragUnifLocation = glGetUniformLocation(App->shader->program, "newColor");
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(fragUnifLocation, 1, color);*/

	glBegin(GL_LINES);
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();
}

void ModuleEnvironment::DrawReferenceAxis() {
	glLineWidth(2.0f);

	// red X
	//int xAxis = glGetUniformLocation(App->shaderExercise->program, "newColor");
	//float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//glUniform4fv(xAxis, 1, red);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	// green Y
	//int yAxis = glGetUniformLocation(App->shaderExercise->program, "newColor");
	//float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	//glUniform4fv(yAxis, 1, green);

	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();

	// blue Z
	//int zAxis = glGetUniformLocation(App->shaderExercise->program, "newColor");
	//float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//glUniform4fv(zAxis, 1, blue);

	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();

	glLineWidth(1.0f);
}


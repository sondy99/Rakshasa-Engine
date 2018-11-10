#ifndef __ModuleShader_H__
#define __ModuleShader_H__

#include "Module.h"

#include "GL/glew.h"

class ModuleShader : public Module
{
public:
	ModuleShader();
	~ModuleShader();
	
	GLuint LoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath);
public:
	GLuint program = 0;
private:
	GLuint CreateShader(const char * shaderStr, const GLenum type);
	char* ReadShaderFile(const char* shaderPath);
	void CheckCompilationErrors(GLuint shader);
};

#endif __ModuleShader_H__



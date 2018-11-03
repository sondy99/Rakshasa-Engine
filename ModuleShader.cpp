#include "ModuleShader.h"

ModuleShader::ModuleShader()
{
}


ModuleShader::~ModuleShader()
{
}


bool ModuleShader::Init()
{
	return true;
}


bool ModuleShader::CleanUp()
{
	return true;
}

GLuint ModuleShader::LoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath) 
{

	GLint compileStatus = GL_FALSE;
	int logLength = 0;

	char* vertexShaderStr = ReadShaderFile(vertexShaderPath);
	char* fragmentShaderStr = ReadShaderFile(fragmentShaderPath);

	if (vertexShaderStr == nullptr || fragmentShaderStr == nullptr) {
		LOG("Error: Reading shaders failed");
		return GL_FALSE;
	}

	GLuint vertexShader = CreateShader(vertexShaderStr, GL_VERTEX_SHADER);
	if (vertexShader == 0)
	{
		LOG("Error: Failed creating vertex failed");
		return GL_FALSE;
	}

	GLuint fragmentShader = CreateShader(fragmentShaderStr, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0)
	{
		LOG("Error: Failed creating fragment shader");
		return GL_FALSE;
	}

	// Compile Program
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &compileStatus);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return GL_TRUE;
}

GLuint ModuleShader::CreateShader(const char* shaderStr, const GLenum type)
{
	GLint compileStatus = GL_FALSE;

	GLuint fragShader = glCreateShader(type);
	glShaderSource(fragShader, 1, &shaderStr, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus) {
		CheckCompilationErrors(fragShader);
		glDeleteShader(fragShader);
		return 0;
	}

	return fragShader;
}

char* ModuleShader::ReadShaderFile(const char* shaderPath) {
	FILE* file;
	errno_t error = fopen_s(&file, shaderPath, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		rewind(file);
		char* shaderData = (char*)malloc(size + 1);
		fread(shaderData, 1, size, file);
		shaderData[size] = 0;
		fclose(file);
		return shaderData;
	}

	return nullptr;
}

void ModuleShader::CheckCompilationErrors(GLuint shader) {
	GLint infoLogLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	GLchar* strInfoLog = new GLchar[infoLogLength + 1];
	glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

	LOG(strInfoLog);

	delete[] strInfoLog;
	infoLogLength = NULL;
	glDeleteShader(shader);
}
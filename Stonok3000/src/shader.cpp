#include <stdlib.h>
#include <stdio.h>


#include <GL/glew.h>

#include "shader.h"

typedef char* TCharBuffer;

//oFragColor = vec4(iNormal + vec3(sin(time), cos(time), sin(time + 2.0)), 1.0);

bool LoadFile(const char path[], TCharBuffer& rpBuffer, size_t& rSize) {
	FILE* pFile = nullptr;
	if (fopen_s(&pFile, path, "rb") != 0)
		return false;

	fseek(pFile, 0, SEEK_END);
	rSize = static_cast<size_t>(ftell(pFile));
	fseek(pFile, 0, SEEK_SET);

	rpBuffer = reinterpret_cast<TCharBuffer>(malloc(rSize + 1));
	if (rpBuffer == nullptr)
		return false;

	fread(rpBuffer, rSize, 1, pFile);
	fclose(pFile);
	
	rpBuffer[rSize] = '\0';

	return true;
}


Shader::Shader() : m_program(0) {
}

Shader::~Shader() {
	if (m_program != 0)
		glDeleteProgram(m_program);
}

bool Shader::Load(const char fragmentPath[], const char vertexPath[]) {
	printf("Loading shader (%s:%s)...\n", fragmentPath, vertexPath);

	TCharBuffer fragmentSource;
	size_t fragmentSourceSize;
	if (LoadFile(fragmentPath, fragmentSource, fragmentSourceSize) == false)
		return false;


	TCharBuffer vertexSource;
	size_t vertexSourceSize;
	if (LoadFile(vertexPath, vertexSource, vertexSourceSize) == false) {
		free(fragmentSource);

		return false;
	}

	bool bCompiled = Compile(fragmentSource, vertexSource);

	free(fragmentSource);
	free(vertexSource);

	return bCompiled;
}

bool Shader::Compile(const char fragmentSource[], const char vertexSource[]) {
	// Выделение, компиляция и проверка фрагментного шейдера
	printf("Compiling fragment...\n");
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	if (CheckShader(fragmentShader, GL_COMPILE_STATUS) == 0)
		return false;

	// Выделение, компиляция и проверка вершинного шейдера
	printf("Compiling vertex...\n");
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	if (CheckShader(vertexShader, GL_COMPILE_STATUS) == 0) {
		glDeleteShader(fragmentShader);
		return false;
	}

	// Выделение шейдерной программы и привязка шейдеров проходов
	m_program = glCreateProgram();
	glAttachShader(m_program, fragmentShader);
	glAttachShader(m_program, vertexShader);

	// Связывание
	glLinkProgram(m_program);

	// Удаление шейдеров проходов (они уже есть в шейдерной программе)
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	// Проверка программы
	if (CheckProgram(m_program, GL_LINK_STATUS) == 0)
		return false;

	return true;
}

void Shader::Use() const {
	glUseProgram(m_program);
}

GLint Shader::GetUniform(const GLchar name[]) {
	return glGetUniformLocation(m_program, name);
}

GLint Shader::CheckShader(GLuint shader, GLenum pname) {
	GLint success;
	glGetShaderiv(shader, pname, &success);
	if (success == 0) {
		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		char* infoLog = new char[logSize];

		glGetShaderInfoLog(shader, logSize, nullptr, infoLog);
		printf("Shader error: %s\n", infoLog);

		delete[] infoLog;

		return 0;
	}

	return success;
}

GLint Shader::CheckProgram(GLuint program, GLenum pname) {
	GLint success;
	glGetProgramiv(program, pname, &success);
	if (success == 0) {
		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* infoLog = new char[logSize];

		glGetProgramInfoLog(program, logSize, nullptr, infoLog);
		printf("Shader error: %s\n", infoLog);

		delete[] infoLog;

		return 0;
	}

	return success;
}
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
	Shader();
	~Shader();

	bool Load(const char fragmentPath[], const char vertexPath[]);
	bool Compile(const char fragmentSource[], const char vertexSource[]);

	void Use() const;
	GLint GetUniform(const GLchar name[]);
private:
	GLint CheckShader(GLuint shader, GLenum pname);
	GLint CheckProgram(GLuint program, GLenum pname);

	GLuint m_program;
};
#endif
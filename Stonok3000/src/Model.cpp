#include "Model.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glfw3.h> 


bool loadmodel(const char path[], std::vector<Vertex>& Vertexes)
{
	FILE* pFile = nullptr;
	if (fopen_s(&pFile, path, "rt") != 0)
		return false;

	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;

	while (feof(pFile) == 0) {
		char buffer[256];

		fgets(buffer, 255, pFile);

		if (buffer[0] == 'v') {
			if (buffer[1] == ' ') {
				Vec3 tmp;
				sscanf_s(buffer, "v %f %f %f", &tmp.x, &tmp.y, &tmp.z);
				
				positions.push_back(tmp);
			}
			else if (buffer[1] == 'n') {
				Vec3 tmp;
				sscanf_s(buffer, "vn %f %f %f", &tmp.x, &tmp.y, &tmp.z);
				
				normals.push_back(tmp);
			}
			else if (buffer[1] == 't') {
				Vec2 tmp;
				sscanf_s(buffer, "vt %f %f", &tmp.x, &tmp.y);
				uvs.push_back(tmp);
			}
		}
		else if (buffer[0] == 'f') {
			int v0, t0, n0;
			int v1, t1, n1;
			int v2, t2, n2;

			sscanf_s(buffer, "f %i/%i/%i %i/%i/%i %i/%i/%i", &v0, &t0, &n0, &v1, &t1, &n1, &v2, &t2, &n2);

			Vertexes.push_back(Vertex{
				positions[v0 - 1],
				normals[n0 - 1],
				uvs[t0 - 1]
				});

			Vertexes.push_back(Vertex{
				positions[v1 - 1],
				normals[n1 - 1],
				uvs[t1 - 1]
				});

			Vertexes.push_back(Vertex{
				positions[v2 - 1],
				normals[n2 - 1],
				uvs[t2 - 1]
				});
		}
	}

	fclose(pFile);

	return true;
}

Model::Model() : m_count(0), m_vao(0), m_vbo(0) {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	// Привязка VBO к VAO
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

Model::~Model() {
	if (m_vao != 0)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo != 0)
		glDeleteBuffers(1, &m_vbo);
}

bool Model::Load(const char path[]) {
	std::vector<Vertex> vertexes;
	//, float mx, float my, float mz
	if (loadmodel("untitled.obj", vertexes) == false)
		return false;

	m_count = vertexes.size();

	// Привязка VBO как текущего с выделением и заполнением памяти (статический режим хранения, то есть предполагается отсуствие дальнейших обновлений буфера)
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex), &vertexes[0].position.x, GL_STATIC_DRAW);

	// Включение атрибутов (нужно, чтобы они стали доступны в шейдере при отрисовке)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Привязка VAO как текущего
	glBindVertexArray(m_vao);

	// Настройка смещения данных атрибутов в VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

	return true;
}

void Model::Draw(GLuint mode) const {
	// Привязка VAO
	glBindVertexArray(m_vao);

	// Отрисовка VBO
	glDrawArrays(mode, 0, static_cast<int>(m_count));
}
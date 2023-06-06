#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <GL/glew.h>;

struct Vec2 {
	float x;
	float y;
};

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};

bool loadmodel(const char path[], std::vector<Vertex>& Vertexes);

class Model {
public:
	Model();
	~Model();

	bool Load(const char path[]);

	void Draw(GLuint mode) const;

private:
	GLuint m_vbo, m_vao;
	size_t m_count;
};
#endif

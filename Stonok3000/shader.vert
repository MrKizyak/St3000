#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 matrix;
uniform mat4 model;
uniform float time;

out vec3 iNormal;

void main() {
	iNormal = normal;

	vec4 coord = model * vec4(position/* + normalize(position) * vec3(sin(time + position.x), cos(time + position.z), sin(time + 2.0 +  position.y))*/, 1.0);
	gl_Position = matrix * coord;
}
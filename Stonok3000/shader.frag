#version 330 core

out vec4 oFragColor;

in vec3 iNormal;

uniform float time;

uniform vec4 ourColor;
void main() {
	oFragColor = ourColor;
}
#version 330 core

layout (location=0) in vec3 in_vertexPosition;

// projection * view * model matrix
uniform mat4 mvp;

out vec2 p;

void main(){
	gl_Position = mvp * vec4(in_vertexPosition, 1);

	p = gl_Position.xy;
}
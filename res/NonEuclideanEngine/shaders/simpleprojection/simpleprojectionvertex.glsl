#version 330 core

layout (location=0) in vec3 in_vertexPosition;

// projection * view * model matrix
uniform mat4 mvp;

out vec3 color;

void main(){
	gl_Position = mvp * vec4(in_vertexPosition, 1);

	color = in_vertexPosition + 0.5;
}


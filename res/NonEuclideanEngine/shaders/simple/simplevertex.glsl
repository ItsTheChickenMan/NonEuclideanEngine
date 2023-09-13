#version 330 core

layout (location=0) in vec3 vertexPosition;

out vec3 color;

void main(){
	gl_Position = vec4(vertexPosition, 1.0);
	
	color = vertexPosition + vec3(0.5);
}
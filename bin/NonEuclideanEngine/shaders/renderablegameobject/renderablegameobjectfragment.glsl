#version 330 core

in vec3 textureCoordinates;
in vec2 p;

out vec4 FragColor;

float rand(vec2 co);

void main(){
	float col = rand(p);
	
	FragColor = vec4(vec3(col), 1);
}

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
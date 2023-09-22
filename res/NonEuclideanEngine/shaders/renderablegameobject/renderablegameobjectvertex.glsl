#version 330 core

layout (location=0) in vec3 in_vertexPosition;
layout (location=1) in vec2 in_textureCoordinates;

// projection * view * model matrix
uniform mat4 u_mvp;

// output texture coordinates
out vec2 TextureCoordinates;

void main(){
	gl_Position = u_mvp * vec4(in_vertexPosition, 1);
	
	TextureCoordinates = in_textureCoordinates;

	// FIXME: we should flip tex coords properly
	TextureCoordinates.y = 1.0 - TextureCoordinates.y;
}
#version 330 core

// in vars
in vec2 TextureCoordinates;

// out vars
out vec4 FragColor;

// texture
uniform sampler2D u_sampler;

void main(){
	vec4 textureColor = texture(u_sampler, TextureCoordinates);

	//FragColor = textureColor;
	FragColor = vec4(TextureCoordinates * vec2(textureColor), 0, 1);
}
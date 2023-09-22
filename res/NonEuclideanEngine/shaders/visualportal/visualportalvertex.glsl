#version 330 core

layout (location=0) in vec3 in_vertexPosition;
layout (location=1) in vec2 in_textureCoordinates;

// projection * view matrix
// model MUST be provided through the separate model uniform for portals to render properly, but this has to stay as u_mvp to stay compatible with RenderableObjectShaderPrograms
uniform mat4 u_mvp;

// base model matrix
uniform mat4 u_model;

// additional model matrix applied to the vertex.  this is not applied to the texture coordinates, allowing the portal to move without changing the texture mapping.
// this is handy for when a portal has to render itself, otherwise it should just be the identity matrix
// TODO: this is pretty slow, there might be a faster way to do this (maybe at the very least precalculate additional model?)
uniform mat4 u_additionalModel;

// output texture coordinates
noperspective out vec2 TextureCoordinates;

void main(){
	gl_Position = u_mvp * u_additionalModel * u_model * vec4(in_vertexPosition, 1);

	vec4 noAdditional = u_mvp * u_model * vec4(in_vertexPosition, 1);

	// we don't want to project the texture onto the surface, but rather sample the texture according to the absolute position of the fragment on the screen so that the texture matches up exactly.
	// we do this by determining where the fragment is on screen using gl_Position and the w component, then relying on the interpolation done by glsl
	// NDC space is in -1.0 to 1.0 while texture coordinates are 0.0 to 1.0, so we normalize that here as well
	// it's also important that we add the noperspective qualifier to ensure that it interpolates in window space, so each fragment has the correct texture coordinate mapping 1-1 with the NDC
	TextureCoordinates = (noAdditional.xy / noAdditional.w)*0.5 + 0.5;
	//TextureCoordinates = in_textureCoordinates;
}
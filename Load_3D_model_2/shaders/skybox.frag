#version 430 core

in vec3 text_coords;

out vec4 f_color;

uniform samplerCube skybox;

void main()
{

	f_color = texture( skybox, text_coords);
	
}
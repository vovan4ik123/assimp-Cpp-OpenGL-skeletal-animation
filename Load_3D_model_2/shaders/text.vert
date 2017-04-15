#version 430
layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec2 in_text_coord;

out vec2 text_coords;

uniform mat4 PROJECTION_matrix;

void main()
{
	gl_Position = PROJECTION_matrix * vec4(in_vertex, 1.0);
	text_coords = in_text_coord;
}
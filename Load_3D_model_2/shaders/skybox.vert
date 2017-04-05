#version 430 core

layout(location = 0) in vec3 in_position;

out vec3 text_coords;

uniform mat4 VP; //   WITHOUT model matrix

void main()
{
	vec4 pos = VP * vec4( in_position, 1.0);
	gl_Position = pos.xyww; // optimization !! its work with glDepthFunc(GL_LEQUAL);
							// and draw skybox after scene !!!

	text_coords = in_position;
}
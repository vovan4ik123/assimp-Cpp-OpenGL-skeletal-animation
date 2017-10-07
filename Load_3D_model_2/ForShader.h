#pragma once

#include "GL\glew.h"
#include "SDL\SDL_opengl.h"

#include <iostream>

class ForShader
{
public:
	ForShader();
	~ForShader();

	static const GLchar* readShader(const char* filename);

	static GLuint makeProgram(const char* vertex, const char* fragment);
};


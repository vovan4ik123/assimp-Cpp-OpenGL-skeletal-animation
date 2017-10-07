#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include <iostream>
#include <map>
#include <string>
// for text rendering
#include "ft2build.h"
#include FT_FREETYPE_H

using namespace std;

struct Character
{
	GLuint texture_id;  // glyph texture
	glm::ivec2 size; // size of glyph
	glm::vec2 bearing;  // offset from the baseline to left/top of glyph
	GLuint  advance;  // offset to advance to next glyph
};

class TextRendering
{
public:
	static TextRendering* Instance()
	{
		static TextRendering inst;
		return &inst;
	}

	void draw(string text, glm::vec3 color, glm::mat4 matrix);

private:
	TextRendering();
	~TextRendering();

	GLuint VAO, VBO;
	GLuint shaders_text;

	map<GLchar, Character> characters;
};


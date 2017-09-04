#include "TextRendering.h"
#include "ForShader.h"
#include "Game.h"

#include <bitset>  

TextRendering::TextRendering()
{
	FT_Library ft_lib;
	if (FT_Init_FreeType(&ft_lib)) cout << "free type init error" << endl;

	FT_Face ft_face;
	if (FT_New_Face(ft_lib, "fonts/ariali.ttf", 0, &ft_face)) cout << "font NOT load" << endl;

	FT_Set_Pixel_Sizes(ft_face, 0, 48); //width to 0 lets the face dynamically calculate the width based on the given height.

										// Disable byte-alignment restriction  ????????????????????
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // use a single byte to represent the colors of a texture (like freetype)

	for (int c = 32; c < 127; c++) //unsigned char 0 - 255 (1 byte = 8 bit)
	{
		//cout << "load: " << (char)c << "  " << c << endl;
		// load character glyph
		if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
		{
			cout << "error load character: " << (char)c << endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,  // изображение, сгенерированное из глифа, является 8-битным изображением,
			ft_face->glyph->bitmap.width,
			ft_face->glyph->bitmap.rows,
			0, GL_RED, // где каждый цвет представлен одним байтом.
			GL_UNSIGNED_BYTE, // этот байт будет влиять ТОЛЬКО НА АЛЬФУ ТЕКСТУРЫ
			ft_face->glyph->bitmap.buffer);

		// texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		// store character in map
		Character character = { texture,
			glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
			glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
			ft_face->glyph->advance.x };

		characters.insert(pair<GLchar, Character>(c, character));
	}

	// clear
	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_lib);

	glBindTexture(GL_TEXTURE_2D, 0);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // use standart 4 byte represents the colors of a texture

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // 1 квадрат = 6 вершин ( 5 флоат значения на каждую вершину ) 
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, NULL, GL_DYNAMIC_DRAW); //  GL_DYNAMIC_DRAW - если часто обновляем память
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shaders_text = ForShader::makeProgram("shaders/text.vert", "shaders/text.frag");
}


TextRendering::~TextRendering()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shaders_text);
}

void TextRendering::draw(string text, glm::vec3 color, glm::mat4 matrix)
{
	glUseProgram(shaders_text);
	glUniformMatrix4fv(glGetUniformLocation(shaders_text, "PROJECTION_matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
	glUniform3f(glGetUniformLocation(shaders_text, "text_color"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	float offset_x = 0.0f; // offset to next char
	float offset_y = 0.0f; // offset to next char

	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		float x_pos = offset_x + ch.bearing.x;
		float y_pos = offset_y - (ch.size.y - ch.bearing.y);

		float w = ch.size.x;
		float h = ch.size.y ;

		// update VBO for each character
		float vertices[6][5] =
		{	// x			// y		// z	// texture
			{ x_pos,		y_pos + h,  0.0f,	0.0, 0.0 },
			{ x_pos,		y_pos,		0.0f,	0.0, 1.0 },
			{ x_pos + w,	y_pos,		0.0f,	1.0, 1.0 },
										
			{ x_pos,		y_pos + h,  0.0f,	0.0, 0.0 },
			{ x_pos + w,	y_pos,		0.0f,	1.0, 1.0 },
			{ x_pos + w,	y_pos + h,  0.0f,	1.0, 0.0 },
		};
		// render texture over this quad
		glBindTexture(GL_TEXTURE_2D, ch.texture_id);
		// update VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//bitset<16> bitset1{ ch.advance }; bitset<16> bitset2{ ch.advance >> 6 };
		//cout <<    bitset1			<< "   " <<		 bitset2 << endl;
		//cout << ch.advance << "   " << (ch.advance >> 1) << endl; // (ch.advance >> 1) == (ch.advance / 2)

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		offset_x += (ch.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	//cout << offset_x << endl;

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

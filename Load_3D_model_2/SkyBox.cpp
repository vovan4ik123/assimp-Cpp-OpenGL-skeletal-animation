#include "SkyBox.h"
#include "ForShader.h"
#include "Game.h"
#include "InputHandler.h"

#include <iostream>
#include <vector>
#include <cstring>


SkyBox::SkyBox()
{

}


SkyBox::~SkyBox()
{
	glDeleteTextures(1, &cube_texture_id);
}

void SkyBox::init(char* folder)
{
	float offset_side_cube = 1.0f;

	GLfloat skybox_vertices[] = {
		// Positions          
		-offset_side_cube,  offset_side_cube, -offset_side_cube, // front
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // left
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		offset_side_cube, -offset_side_cube, -offset_side_cube, // right
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // back
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		-offset_side_cube,  offset_side_cube, -offset_side_cube, // top
		 offset_side_cube,  offset_side_cube, -offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		 offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  -offset_side_cube,

		-offset_side_cube, -offset_side_cube, -offset_side_cube, // bottom
		 offset_side_cube, -offset_side_cube, -offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		 offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube, -offset_side_cube
	};

	// VBO
	glGenBuffers(1, &VBO_vertices_textures);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// VAO
	glGenVertexArrays(1, &VAO_skybox);
	glBindVertexArray(VAO_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cube_texture_id = createCubeTexture(folder);

	skybox_shaders = ForShader::makeProgram("shaders/skybox.vert", "shaders/skybox.frag");

}

void SkyBox::update(glm::mat4 VP_matr)
{
	// another skybox

	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_5))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_morning");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_6))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_ayden");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_7))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_drakeq");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_8))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_mercury");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_9))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_shadow");
	}
	if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_0))
	{
		glDeleteTextures(1, &cube_texture_id);
		cube_texture_id = createCubeTexture("images/skybox_violentday");
	}

	// get MV matrix from world around whom draw skybox
	VP_matrix = VP_matr;
}

void SkyBox::draw()
{

	// draw skybox LAST in scene ( optimization in vertex shader )
	glDepthFunc(GL_LEQUAL); // optimization in shaders
	glUseProgram(skybox_shaders);
	glBindVertexArray(VAO_skybox);
	glUniformMatrix4fv(glGetUniformLocation(skybox_shaders, "VP"), 1, GL_FALSE, glm::value_ptr(VP_matrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS); // return to default
}

GLuint SkyBox::createCubeTexture(char* folder)
{
	GLuint text_id;
	glGenTextures(1, &text_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, text_id);

	std::vector<string> image_path; // same name single textures must be in folder
	image_path.push_back(string(folder) + string("/right.dds"));
	image_path.push_back(string(folder) + string("/left.dds"));
	image_path.push_back(string(folder) + string("/top.dds"));
	image_path.push_back(string(folder) + string("/bottom.dds"));
	image_path.push_back(string(folder) + string("/back.dds"));
	image_path.push_back(string(folder) + string("/front.dds"));

	for (int i = 0; i < 6; i++) // load DDS image for each side cube map
	{


		unsigned char header[124];
		// пробуем открыть файл
		FILE *fp;
		fp = fopen(image_path[i].c_str(), "rb");
		if (fp == NULL)
		{
			std::cout << " image not load \n";
			return 0;
		}

		// проверим тип файла
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return 0;
		}

		// читаем заголовок
		fread(&header, 124, 1, fp);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linear_size = *(unsigned int*)&(header[16]);
		unsigned int mipmap_count = *(unsigned int*)&(header[24]);
		unsigned int four_cc = *(unsigned int*)&(header[80]);  // formats

#ifdef _DEBUG
		std::cout << "image: " << image_path[i].c_str() << " mipmap_count = " << mipmap_count << std::endl;
#endif // _DEBUG


		unsigned char *buffer;
		unsigned int buff_size = mipmap_count > 1 ? linear_size * 2 : linear_size;
		buffer = (unsigned char*)malloc(buff_size * sizeof(unsigned char));
		fread(buffer, 1, buff_size, fp);
		// закрываем файл
		fclose(fp);

#define FOURCC_DXT1 0x31545844 // "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // "DXT5" in ASCII

		unsigned int components = (four_cc == FOURCC_DXT1) ? 3 : 4;;
		unsigned int format;
		if (four_cc == FOURCC_DXT1)
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		else if (four_cc == FOURCC_DXT3)
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		else if (four_cc == FOURCC_DXT5)
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

#undef FOURCC_DXT1
#undef FOURCC_DXT3
#undef FOURCC_DXT5

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		if (mipmap_count == 1) // if we have only 1 image 0 level ( not mipmap )
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);	// highest resolution
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);	// lowest resolution
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else // we have mipmap
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);	//  highest resolution
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmap_count);	// lowest resolution
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;;
		unsigned int offset = 0;

		// load mipmap. if we have not mipmap mipmap_count = 1 and be loaded 1 image (0 level)
		for (unsigned int level = 0; level < mipmap_count && (width || height); level++)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, format, width, height, 0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;
		}
		free(buffer);

	}

	return text_id;
}


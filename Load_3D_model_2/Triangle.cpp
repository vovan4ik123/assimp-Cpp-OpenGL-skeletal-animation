#include "Triangle.h"
#include "Game.h"
#include "SkyBox.h"
#include "TextRendering.h"
#include "ForShader.h"
#include "IL\il.h"
#include "IL\ilu.h"
#include "IL\ilut.h"

Triangle::Triangle()
{
	cout << "triangle construktor()" << endl;

	glm::vec3 position(2.0f, 5.0f, 18.0f);
	camera = Camera(position);

	mouse_first_in = true;
}


Triangle::~Triangle()
{
	glDeleteProgram(shaders_animated_model);
}


void Triangle::init()
{

	// shader for animated model
	shaders_animated_model = ForShader::makeProgram("shaders/animated_model.vert", "shaders/animated_model.frag");

	model_man.initShaders(shaders_animated_model);
	model_man.loadModel("models/man/model.dae");
	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model_1 = glm::rotate(matr_model_1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	model_astroboy.loadModel("models/astroboy/astroBoy_walk_Max.dae");
	model_astroboy.initShaders(shaders_animated_model);
	matr_model_2 = glm::rotate(matr_model_2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//matr_model2 = glm::scale(matr_model, glm::vec3(0.3f, 0.3f, 0.3f));
	matr_model_2 = glm::translate(matr_model_2, glm::vec3(5.0f, 0.0f, 0.0f));

	// skybox
	SkyBox::Instance()->init("images/skybox_violentday");

	// text 2D
	text_matrix_2D = glm::ortho(0.0f, (float)Game::Instance()->screen_width, 0.0f, (float)Game::Instance()->screen_height, 1.0f, -1.0f);

}

void Triangle::update()
{
	GLfloat current_frame = SDL_GetTicks();
	delta_time = (current_frame - last_frame);
	last_frame = current_frame;

	// camera
	camera.updateKey(delta_time, speed);
	// mouse
	if (InputHandler::Instance()->getMouseButtonState(LEFT_PRESSED) )
	{
		SDL_ShowCursor(SDL_DISABLE);
		mouse_position = InputHandler::Instance()->getMousePosition();

		if (mouse_first_in) // need run ONLY if mouse on window !!!
		{
			last_x = mouse_position.getX();
			last_y = mouse_position.getY();
			mouse_first_in = false;
		}
		//std::cout << mouse_position.getX()  <<  "		"  << mouse_position.getY() << std::endl;
		GLfloat x_offset = mouse_position.getX() - last_x;
		GLfloat y_offset = mouse_position.getY() - last_y;
		last_x = mouse_position.getX();
		last_y = mouse_position.getY();

		camera.updateMouse(x_offset, y_offset);
	}
	if (InputHandler::Instance()->getMouseButtonState(LEFT_RELEASED))
	{
		SDL_ShowCursor(SDL_ENABLE);
		mouse_first_in = true;
	}

	perspective_view = camera.getViewMatrix();
	perspective_projection = glm::perspective(glm::radians(camera.fov), (float)Game::Instance()->screen_width / (float)Game::Instance()->screen_height, 1.0f, 2000.0f); // пирамида

	model_man.update();
	//our_model2.update();

	// model 1 matrix (MAN)
	matr_model_1 = glm::translate(matr_model_1, glm::vec3(0.0f, -0.05f, 0.0f));
	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model_1 = glm::rotate(matr_model_1, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));

	// text 3D
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(0.02f, 0.02f, 0.0f));
	glm::mat4 set_text_to_origin = glm::translate(glm::mat4(), glm::vec3(-1.8f, -0.4f, 0.0f));
	glm::mat4 text_rotate_y = glm::rotate(glm::mat4(), glm::radians(-camera.yaw - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 text_rotate_x = glm::rotate(glm::mat4(), glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	
	glm::mat4 text_translate_to_model_1 = glm::translate(glm::mat4(), glm::vec3(matr_model_1[3].x, matr_model_1[3].y + 10.0f, matr_model_1[3].z));
	text_matrix_3D_model_1 = perspective_projection * perspective_view * text_translate_to_model_1 * text_rotate_y * text_rotate_x * set_text_to_origin * scale;

	glm::mat4 text_translate_to_model_2 = glm::translate(glm::mat4(), glm::vec3(matr_model_2[3].x, matr_model_2[3].y + 7.0f, matr_model_2[3].z));
	text_matrix_3D_model_2 = perspective_projection * perspective_view * text_translate_to_model_2 * text_rotate_y * text_rotate_x * set_text_to_origin * scale;

	// delete translation from view matrix
	SkyBox::Instance()->update(perspective_projection * glm::mat4(glm::mat3(perspective_view)));
}

void Triangle::render()
{

	glUseProgram(shaders_animated_model);

	glUniform3f(glGetUniformLocation(shaders_animated_model, "view_pos"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "material.shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "material.transparency"), 1.0f);
	// Point Light 1
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.position"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);

	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaders_animated_model, "point_light.specular"), 1.0f, 1.0f, 1.0f);

	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.linear"), 0.007);	//0.14 0.09  0.07  0.045  0.027  0.022  0.014  0.007  0.0014 -	разное расстояние затухания
	glUniform1f(glGetUniformLocation(shaders_animated_model, "point_light.quadratic"), 0.0002);//0.07 0.032 0.017 0.0075 0.0028 0.0019 0.0007 0.0002 0.000007	расстояние -->

	MVP = perspective_projection * perspective_view * matr_model_1;
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model_1));
	glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(matr_model_1)));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube));
	model_man.draw(shaders_animated_model);

	MVP = perspective_projection * perspective_view * matr_model_2;
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model_2));
	glm::mat4 matr_normals_cube2 = glm::mat4(glm::transpose(glm::inverse(matr_model_2)));
	glUniformMatrix4fv(glGetUniformLocation(shaders_animated_model, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube2));
	model_astroboy.draw(shaders_animated_model);
	glUseProgram(0);

	// draw skybox after scene
	SkyBox::Instance()->draw();

	glDepthFunc(GL_ALWAYS);
	// draw text after skybox so that antialiasing for lines works
	// text 2D
	glm::mat4 translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 65.0f, 0.0f));
	glm::mat4 scale_2d_text = glm::scale(glm::mat4(), glm::vec3(0.5f, 0.5f, 0.5f));
	TextRendering::Instance()->draw("Buttons: 1,2,3,4 = controls head Agent_1", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);
	
	translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 35.0f, 0.0f));
	TextRendering::Instance()->draw("Buttons: 5,6,7,8,9,0 = change skybox", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);

	translate_2d_text = glm::translate(glm::mat4(), glm::vec3(20.0f, 5.0f, 0.0f));
	TextRendering::Instance()->draw("Buttons: W, S, A, D, SPACE = move  (ALT + F4 = stop)", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_2D * translate_2d_text * scale_2d_text);

	// text 3D 
	TextRendering::Instance()->draw("Agent_1", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_3D_model_1);
	TextRendering::Instance()->draw("Agent_2", glm::vec3(0.1f, 1.0f, 0.0f), text_matrix_3D_model_2);

	glDepthFunc(GL_LESS);

	// music
	music1 = Mix_LoadMUS("music/modern_talking_jet_airliner.mp3");
	Mix_VolumeMusic(16);
	Mix_PlayMusic(music1, 0); // -1 = NONSTOP playing

	music2 = Mix_LoadMUS("music/02_modern_talking_you_can_win_if_you_want.mp3");

}

void Triangle::playSound()
{
	if (Mix_PlayingMusic() == 0)
		Mix_PlayMusic(music2, 1); // play next ( two time <- loop == 1 )

	model_man.playSound();
}

GLuint Triangle::loadImageToTexture(const char* image_path)
{

	ILuint ImageName; // The image name to return.
	ilGenImages(1, &ImageName); // Grab a new image name.
	ilBindImage(ImageName); // загрузит фотку в прикрепленную имаге
	if (!ilLoadImage((ILstring)image_path)) std::cout << "image NOT load " << std::endl;
	// we NEED RGB image (for not transparent)
	//ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); //Convert image to RGBA with unsigned byte data type

	GLuint textureID;
	glGenTextures(1, &textureID); // создать текстуру
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	ilDeleteImages(1, &ImageName);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint Triangle::loadDDS(const char* image_path, int *w, int *h)
{
	// for use this method better way :
	// compress your image, save like .DDS ( with photoshop and nvidia utilite )
	// give address image to this method
	// method returns complete !! OpenGL texture for drawing on screen
	unsigned char header[124];

	// пробуем открыть файл
	FILE *fp;
	fp = fopen(image_path, "rb");
	if (fp == NULL)
	{
		std::cout << "image not load \n";
		return 0;
	}

	// проверим тип файла
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		std::cout << "return 0; \n";
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

	if (w != nullptr) *w = width;
	if (h != nullptr) *h = height;

#ifdef _DEBUG
	std::cout << "image: " << image_path << " mipmap_count = " << mipmap_count << std::endl;
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);	// highest resolution
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);	// lowest resolution
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else // we have mipmap
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);	//  highest resolution
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap_count);	// lowest resolution
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;;
	unsigned int offset = 0;

	// load mipmap. if we have not mipmap mipmap_count = 1 and be loaded 1 image (0 level)
	for (unsigned int level = 0; level < mipmap_count && (width || height); level++)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
	}
	free(buffer);

	return textureID;
}
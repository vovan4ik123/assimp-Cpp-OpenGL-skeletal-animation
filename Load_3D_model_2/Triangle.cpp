#include "Triangle.h"
#include "Game.h"
#include "SkyBox.h"

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

}


void Triangle::init()
{

	// shader for animated model
	programID_scene = ForShader::makeProgram("shaders/animated_model.vert", "shaders/animated_model.frag");

	our_model.initShaders(programID_scene);
	our_model.loadModel("models/man/model.dae");
	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model = glm::rotate(matr_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	our_model2.loadModel("models/astroboy/astroBoy_walk_Max.dae");
	our_model2.initShaders(programID_scene);
	matr_model2 = glm::rotate(matr_model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//matr_model2 = glm::scale(matr_model, glm::vec3(0.3f, 0.3f, 0.3f));
	matr_model2 = glm::translate(matr_model2, glm::vec3(5.0f, 0.0f, 0.0f));

	// skybox
	SkyBox::Instance()->init("images/skybox_violentday");
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

	our_model.update();

	// delete translation from view matrix
	SkyBox::Instance()->update(perspective_projection * glm::mat4(glm::mat3(perspective_view)));
}

void Triangle::render()
{

	glUseProgram(programID_scene);

	glUniform3f(glGetUniformLocation(programID_scene, "view_pos"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);
	glUniform1f(glGetUniformLocation(programID_scene, "material.shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(programID_scene, "material.transparency"), 1.0f);
	// Point Light 1
	glUniform3f(glGetUniformLocation(programID_scene, "point_light.position"), camera.camera_pos.x, camera.camera_pos.y, camera.camera_pos.z);

	glUniform3f(glGetUniformLocation(programID_scene, "point_light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(programID_scene, "point_light.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(programID_scene, "point_light.specular"), 1.0f, 1.0f, 1.0f);

	glUniform1f(glGetUniformLocation(programID_scene, "point_light.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(programID_scene, "point_light.linear"), 0.007);	//0.14 0.09  0.07  0.045  0.027  0.022  0.014  0.007  0.0014 -	разное расстояние затухания
	glUniform1f(glGetUniformLocation(programID_scene, "point_light.quadratic"), 0.0002);//0.07 0.032 0.017 0.0075 0.0028 0.0019 0.0007 0.0002 0.000007	расстояние -->

	matr_model = glm::translate(matr_model, glm::vec3(0.0f, -0.05f, 0.0f));
	//matr_model = glm::scale(matr_model, glm::vec3(0.1f, 0.1f, 0.1f));
	matr_model = glm::rotate(matr_model, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));

	MVP = perspective_projection * perspective_view * matr_model;
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model));
	glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(matr_model)));
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube));
	our_model.draw(programID_scene);

	MVP = perspective_projection * perspective_view * matr_model2;
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "M_matrix"), 1, GL_FALSE, glm::value_ptr(matr_model2));
	glm::mat4 matr_normals_cube2 = glm::mat4(glm::transpose(glm::inverse(matr_model2)));
	glUniformMatrix4fv(glGetUniformLocation(programID_scene, "normals_matrix"), 1, GL_FALSE, glm::value_ptr(matr_normals_cube2));
	our_model2.draw(programID_scene);
	glUseProgram(0);

	// draw skybox after scene
	SkyBox::Instance()->draw();

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

	our_model.playSound();
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
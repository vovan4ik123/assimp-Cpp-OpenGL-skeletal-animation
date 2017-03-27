#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Triangle
{
public:
	Triangle();
	~Triangle();

	GLfloat speed = 0.005f;
	//for camera
	GLfloat delta_time = 0.0f;
	GLfloat last_frame = 0.0f;
	GLfloat last_x;
	GLfloat last_y;
	Vector2D mouse_position;
	GLboolean mouse_first_in;
	Camera camera;

	GLuint programID_scene;
	Model our_model;
	Model our_model2;
	Model our_model3;

	glm::mat4 MVP;
	glm::mat4 perspective_view;
	glm::mat4 perspective_projection;
	glm::mat4 matr_model;
	glm::mat4 matr_model2;
	glm::mat4 matr_model3;

	void init();
	void update();
	void render();
	void playSound();

	static GLuint loadDDS(const char* image_path, int* w = nullptr, int* h = nullptr);
	static GLuint loadImageToTexture(const char* image_path);

};


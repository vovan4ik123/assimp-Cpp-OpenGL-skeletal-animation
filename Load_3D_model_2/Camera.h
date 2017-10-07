#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "InputHandler.h"
#include "Vector2D.h"

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat FOV = 45.0f;

class Camera
{
public:
	~Camera() {};


	glm::vec3 camera_pos;
	glm::vec3 camera_front;
	glm::vec3 camera_up;
	glm::vec3 camera_right;
	glm::vec3 world_up;

	GLfloat sensitivity;
	GLfloat yaw; // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	GLfloat pitch;
	GLfloat fov;

	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) : 
		camera_front(glm::vec3(0.0f, 0.0f, -1.0f)), yaw(YAW), pitch(PITCH), fov(FOV), sensitivity(SENSITIVTY)
	{
		camera_pos = pos;
		world_up = up;

		updateCameraVectors();
	}

	void updateKey(GLfloat delta_time, GLfloat speed)
	{
		GLfloat velocity = speed * delta_time;
		//std::cout << " velocity = " << velocity << std::endl;
		// mowe
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W))
		{
			camera_pos += velocity * camera_front;
			//std::cout << " velocity = " << velocity << std::endl;
		}
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S))
		{
			camera_pos -= velocity / 2 * camera_front;
			//std::cout << " velocity /2 = " << velocity /2 << std::endl;
		}
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A))
		{
			camera_pos -= camera_right * velocity;
		}
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D))
		{
			camera_pos += camera_right * velocity;
		}
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_SPACE))
		{
			camera_pos += glm::vec3(0.0f, 0.01f * delta_time, 0.0f); // for jump
		}
		// zoom
		if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_LSHIFT))
		{
			fov = 10.0f;
			sensitivity = 0.025f;
		}
		else
		{
			fov = 45.0f;
			sensitivity = 0.1f;
		}

	}

	void updateMouse(GLfloat x_offset, GLfloat y_offset)
	{
		x_offset *= sensitivity;
		y_offset *= sensitivity;
		yaw += x_offset;
		pitch -= y_offset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		camera_front = glm::normalize(front);

		camera_right = glm::normalize(glm::cross(camera_front, world_up)); // cross product is a vector perpendicular to both vectors
		camera_up = glm::normalize(glm::cross(camera_right, camera_front));
	}
};
#pragma once

#include "Vector2D.h"

#include <iostream>
#include <vector>
#include <SDL\SDL.h>

enum mouse_buttons
{
	LEFT_PRESSED = 0,
	MIDDLE_PRESSED = 1,
	RIGHT_PRESSED = 2,

	LEFT_RELEASED = 3,
	MIDDLE_RELEASED = 4,
	RIGHT_RELEASED = 5
};

class InputHandler
{
public:
	static InputHandler* Instance()
	{
		static InputHandler inst;
		return &inst;
	}

	void updateEvent();
	void clean();

	bool getMouseButtonState(int buttonNumber)
	{
		return m_mouseButtonState[buttonNumber];
	}

	Vector2D getMousePosition()
	{
		return m_mousePosition;
	}

	bool isKeyDown(SDL_Scancode key);
	bool isKeyNotDown(SDL_Scancode key); // for detected button release after press

	void resetMouse();


private:
	InputHandler();

	std::vector<bool> m_mouseButtonState;
	Vector2D m_mousePosition;

	// handle mouse events
	void onMouseMove(SDL_Event& event);
	void onMouseButtonDown(SDL_Event& event);
	void onMouseButtonUp(SDL_Event& event);

	// keyboard
	const Uint8* h_keyStates;


};


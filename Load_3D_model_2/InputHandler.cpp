#include "InputHandler.h"
#include "Game.h"


InputHandler::InputHandler()
{
	m_mouseButtonState.reserve(6);
	for (int i = 0; i < 6; i++)  // for 3 mouse button by 2 states
	{
		m_mouseButtonState.push_back(false);
	}
}


void InputHandler::updateEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		h_keyStates = SDL_GetKeyboardState(0); // vernet massiw s sostojaniem knopok
		switch (event.type)
		{
		case SDL_QUIT:
			Game::Instance()->clean();
			break;

		case SDL_MOUSEBUTTONDOWN:
			onMouseButtonDown(event);
			break;

		case SDL_MOUSEBUTTONUP:
			onMouseButtonUp(event);
			break;

		case SDL_MOUSEMOTION:
			onMouseMove(event);
			break;

		//case SDL_KEYDOWN:
		//	buttonPress(event);
		//	break;

		//case SDL_KEYUP:
		//	buttonReleased(event);
		//	break;
		}
	}
}

void InputHandler::onMouseMove(SDL_Event& event)
{
	m_mousePosition.setX(event.motion.x);
	m_mousePosition.setY(event.motion.y);

}

void InputHandler::onMouseButtonDown(SDL_Event& event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonState[LEFT_PRESSED] = true;
		m_mouseButtonState[LEFT_RELEASED] = false;
	}
	if (event.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonState[MIDDLE_PRESSED] = true;
		m_mouseButtonState[MIDDLE_RELEASED] = false;
	}
	if (event.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonState[RIGHT_PRESSED] = true;
		m_mouseButtonState[RIGHT_RELEASED] = false;
	}
}

void InputHandler::onMouseButtonUp(SDL_Event& event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		m_mouseButtonState[LEFT_RELEASED] = true;
		m_mouseButtonState[LEFT_PRESSED] = false;
	}
	if (event.button.button == SDL_BUTTON_MIDDLE)
	{
		m_mouseButtonState[MIDDLE_RELEASED] = true;
		m_mouseButtonState[MIDDLE_PRESSED] = false;
	}
	if (event.button.button == SDL_BUTTON_RIGHT)
	{
		m_mouseButtonState[RIGHT_RELEASED] = true;
		m_mouseButtonState[RIGHT_PRESSED] = false;
	}
}

void InputHandler::resetMouse()
{
	m_mouseButtonState[LEFT_PRESSED] = false;
	m_mouseButtonState[MIDDLE_PRESSED] = false;
	m_mouseButtonState[RIGHT_PRESSED] = false;

	m_mouseButtonState[LEFT_RELEASED] = false;
	m_mouseButtonState[MIDDLE_RELEASED] = false;
	m_mouseButtonState[RIGHT_RELEASED] = false;
}

bool InputHandler::isKeyDown(SDL_Scancode key)
{
	if (h_keyStates != 0)
	{
		if (h_keyStates[key] == 1)
		{
			return true;
		}

	}
	return false;
}

bool InputHandler::isKeyNotDown(SDL_Scancode key)
{
	if (h_keyStates != 0)
	{
		if (h_keyStates[key] == 1)
		{
			return true;
		}

	}
	return false;
}

void InputHandler::clean()
{

}
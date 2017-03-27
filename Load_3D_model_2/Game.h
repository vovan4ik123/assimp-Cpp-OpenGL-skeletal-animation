#pragma once

#include "Triangle.h"
#include <SDL\SDL.h>

#include <vector>

class Game
{
public:

	void init();
	void render();
	void handleEvents();
	void update();
	void playSound();
	void clean();

	int screen_width;
	int screen_height;


	bool isRunning() { return running; }
	SDL_Window* getWindow() const { return window; }

	static Game* Instance()
	{
		static Game inst;
		return &inst;
	}

private:
	Game();
	~Game();

	SDL_Window* window = 0;
	SDL_GLContext glcontext = 0;

	Triangle triangle;

	bool running;

};


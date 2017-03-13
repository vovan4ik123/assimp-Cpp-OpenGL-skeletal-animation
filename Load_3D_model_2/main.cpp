#include <iostream>
#include <string>

#include "Game.h"
#include <SDL\SDL.h>
#include "GL\freeglut.h"

#define vova
#define PARAM

// 60 powtorenij w sec = kazdyje 16 milisec
const int FPS = 60;
const int DELAY_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec

// framebuffer - часть памяти видеокарты в которой хранятся пиксели которые отобразятся на мониторе.


int main(int argc, char* argv[])
{
	//glutInit(&argc, argv);
	//glutInitContextVersion(4, 3);
	//glutInitContextProfile(GLUT_CORE_PROFILE);

#ifdef vova
	std::cout << "vova define" <<std::endl;
#else
	std::cout << "vova NOT define" << std::endl;
#endif // vova

#if defined(PARAM)
	std::cout << "PARAM define" << std::endl;
#elif defined(DEBUG)
	std::cout << "DEBUG define" << std::endl;
#else
	std::cout << "DEBUG NOT define" << std::endl;
#endif // defined(PARAM)

#ifdef _DEBUG
	std::cout << "debug ON" << std::endl;
#endif // DEBUG


	Uint32 frameStart, frameTime;

	Game::Instance()->init();

	while (Game::Instance()->isRunning())
	{

		frameStart = SDL_GetTicks();

		Game::Instance()->handleEvents();
		Game::Instance()->update();
		Game::Instance()->render();

		frameTime = SDL_GetTicks() - frameStart; // time for full 1 loop
		//std::cout <<" time for loop = " << frameTime << " milli sec"<< std::endl;

		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}
	Game::Instance()->clean();


	return 0;

}
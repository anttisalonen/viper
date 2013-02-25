#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "Plane.h"

class App;
class InputHandler;

class Game {
	public:
		Game(App* app, InputHandler* ih);
		~Game();
		bool update(float frameTime);

	private:
		App* mApp;
		InputHandler* mInputHandler;
		std::list<Plane*> mPlanes;
};

#endif


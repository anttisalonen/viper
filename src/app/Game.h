#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "common/Vector3.h"
#include "common/Quaternion.h"

class App;
class InputHandler;
class Plane;
class Missile;

class Game {
	public:
		Game(App* app, InputHandler* ih);
		~Game();
		bool update(float frameTime);
		std::list<Plane*>& getPlanes();

	private:
		Plane* addPlane(const Common::Vector3& pos, const Common::Quaternion& q);

		App* mApp;
		InputHandler* mInputHandler;
		std::list<Plane*> mPlanes;
		std::list<Missile*> mMissiles;
		Plane* mTrackingPlane;
};

#endif


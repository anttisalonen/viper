#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "common/Clock.h"
#include "common/Vector3.h"
#include "common/Quaternion.h"

class UserInterface;
class InputHandler;
class Plane;
class Missile;

class Game {
	public:
		Game();
		~Game();
		void go();
		bool update(float frameTime);
		std::list<Plane*>& getPlanes();

	private:
		Plane* addPlane(const Common::Vector3& pos, const Common::Quaternion& q);

		InputHandler* mInputHandler;
		UserInterface* mUserInterface;
		std::list<Plane*> mPlanes;
		std::list<Missile*> mMissiles;
		Plane* mTrackingPlane;

		Common::Clock mFPSTimer;
};

#endif


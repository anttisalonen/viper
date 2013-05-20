#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "common/Clock.h"
#include "common/Vector3.h"
#include "common/Quaternion.h"

class UserInterface;
class InputHandler;
class Vehicle;
class Plane;
class SAM;
class Missile;
class Terrain;

class Game {
	public:
		Game();
		~Game();
		void go();
		bool update(float frameTime);
		std::list<Vehicle*>& getVehicles();
		const Terrain* getTerrain() const;

	private:
		Plane* addPlane(const Common::Vector3& pos, const Common::Quaternion& q);
		SAM* addSAM(const Common::Vector3& pos, float dir);

		InputHandler* mInputHandler;
		UserInterface* mUserInterface;
		Terrain* mTerrain;
		std::list<Vehicle*> mVehicles;
		std::list<Missile*> mMissiles;
		Vehicle* mTrackingVehicle;

		Common::Clock mFPSTimer;
};

#endif


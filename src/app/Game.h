#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "common/Clock.h"
#include "common/Vector3.h"
#include "common/Quaternion.h"

class UserInterface;
class InputHandler;
class Vehicle;
enum class VehicleType;
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
		void toggleInputHandler();

	private:
		Vehicle* addVehicle(VehicleType t, int side);

		InputHandler* mInputHandler;
		UserInterface* mUserInterface;
		Terrain* mTerrain;
		std::list<Vehicle*> mVehicles;
		std::list<Missile*> mMissiles;
		Vehicle* mTrackingVehicle;

		Common::Clock mFPSTimer;

		Common::Vector3 mBase[2];
		int mWinner = -1;
		float mBaseWidth = 0.0f;
		float mBaseLength = 0.0f;
};

#endif


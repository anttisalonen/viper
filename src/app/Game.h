#ifndef APP_GAME_H
#define APP_GAME_H

#include <Ogre.h>

#include "common/Clock.h"
#include "common/Vector3.h"
#include "common/Quaternion.h"

class UserInterface;
class InputHandler;
class UserInput;
class GeneralInput;
class Vehicle;
enum class VehicleType;
class Plane;
class SAM;
class Missile;
class Terrain;
class VehicleController;
class General;

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
		bool assignController(VehicleController* c, Vehicle* v);
		bool tryAssignController(VehicleController* c);
		bool tryAssignVehicle(Vehicle* v);
		Vehicle* selectVehicleAt(float x, float y, int side);

		UserInput* mUserInput;
		GeneralInput* mGeneralInput;
		InputHandler* mInputHandler;
		std::vector<VehicleController*> mAIControllers;
		UserInterface* mUserInterface;
		Terrain* mTerrain;
		std::list<Vehicle*> mVehicles;
		std::list<Missile*> mMissiles;
		Vehicle* mTrackingVehicle;

		Common::Clock mFPSTimer;

		Common::Vector3 mBase[2];
		General* mGenerals[2];
		int mWinner = -1;
		float mBaseWidth = 0.0f;
		float mBaseLength = 0.0f;
};

#endif


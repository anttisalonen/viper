#ifndef APP_VEHICLE_H
#define APP_VEHICLE_H

#include <vector>

#include "common/Vector3.h"
#include "common/Quaternion.h"
#include "common/Clock.h"

#include "Entity.h"

class PlaneController;
class Missile;
class Game;

class Vehicle : public VisibleEntity {
	public:
		Vehicle(Game* g, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
		void setTargetVelocity(PrincipalAxis a, float v);
		void setController(PlaneController* p);
		void shoot();
		void addMissile(Missile* m);
		void destroy();
		bool isDestroyed() const;

	protected:
		Game* getGame();
		float mRotationTargetVelocities[3];
		bool grounded() const;

	private:
		void checkShooting();

		Game* mGame = nullptr;
		PlaneController* mController = nullptr;
		bool mShooting = false;
		std::vector<Missile*> mMissiles;
		Vehicle* mTarget = nullptr;
		Common::Countdown mTargetUpdateTimer = Common::Countdown(0.1f);
		bool mDestroyed = false;
		bool mGrounded = false;
};

#endif

#ifndef APP_PLANE_H
#define APP_PLANE_H

#include <vector>

#include "common/Vector3.h"
#include "common/Quaternion.h"
#include "common/Clock.h"

#include "Entity.h"

class PlaneController;
class Missile;
class Game;

class Plane : public VisibleEntity {
	public:
		Plane(Game* g, const Common::Vector3& pos, const Common::Quaternion& q = Common::Quaternion(0, 0, 0, 1));
		virtual void update(float t) override;
		void setTargetVelocity(PrincipalAxis a, float v);
		void setController(PlaneController* p);
		void shoot();
		void addMissile(Missile* m);
		void destroy();
		bool isDestroyed() const;
		virtual const char* getType() const override;

	private:
		void checkShooting();

		Game* mGame = nullptr;
		PlaneController* mController = nullptr;
		float mRotationTargetVelocities[3];
		bool mShooting = false;
		std::vector<Missile*> mMissiles;
		Plane* mTarget = nullptr;
		Common::Countdown mTargetUpdateTimer = Common::Countdown(0.1f);
		bool mDestroyed = false;
};

#endif


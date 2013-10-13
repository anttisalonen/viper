#ifndef APP_VEHICLE_H
#define APP_VEHICLE_H

#include <vector>

#include "common/Vector3.h"
#include "common/Quaternion.h"
#include "common/Clock.h"

#include "Entity.h"

class VehicleController;
class Missile;
class Game;

enum class VehicleType {
	Plane,
	SAM,
	Tank,
};

class Vehicle : public VisibleEntity {
	public:
		Vehicle(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
		void setTargetVelocity(PrincipalAxis a, float v);
		void setTargetVelocity(float v);
		VehicleController* setController(VehicleController* p);
		const VehicleController* getController() const;
		void shoot();
		bool toggleBraking();
		virtual bool acceptsMissiles() const;
		virtual void addMissile(Missile* m);
		void destroy();
		bool isDestroyed() const;
		int getSide() const;
		bool grounded() const;
		virtual VehicleType getVehicleType() const = 0;
		void setTurretRotation(const Common::Quaternion& q);

	protected:
		Game* getGame();
		const Game* getGame() const;
		float mRotationTargetVelocities[3];
		float mTargetVelocity = 0.0f;
		bool mBraking = false;
		std::vector<Missile*> mMissiles;
		int mShells = 0;

	private:
		void checkShooting();
		void setRotationToGround();
		float getHeightAt(float x, float y) const;

		Game* mGame = nullptr;
		int mSide = 0;
		VehicleController* mController = nullptr;
		bool mShooting = false;
		Vehicle* mTarget = nullptr;
		Common::Countdown mTargetUpdateTimer = Common::Countdown(0.1f);
		bool mDestroyed = false;
		bool mGrounded = false;
		std::vector<PhysicalEntity> mProjectiles;
		Common::Quaternion mTurretRotation;
};

#endif

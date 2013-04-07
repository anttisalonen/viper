#include <string.h>
#include <stdexcept>

#include "Vehicle.h"
#include "Missile.h"
#include "Game.h"

#include "PlaneController.h"

#include "common/Math.h"

Vehicle::Vehicle(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: VisibleEntity(pos, q, 10.0f),
	mGame(g)
{
	memset(mRotationTargetVelocities, 0x00, sizeof(mRotationTargetVelocities));
	mVelocity = Common::Vector3(0, 0, 10);
}

void Vehicle::update(float t)
{
	if(mDestroyed) {
		Common::Quaternion tgtRot = Common::Quaternion(sqrt(0.5), 0, 0, sqrt(0.5));
		mRotation = mRotation.slerp(tgtRot,
				Common::clamp(0.0f, 0.3f * t, 1.0f));
		mRotationVelocities[0] = 0.0f;
		mRotationVelocities[1] = 0.0f;
		mRotationVelocities[2] = 0.3f;
	} else {
		// update rotation velocity
		Common::Quaternion rot(0, 0, 0, 1);
		for(int i = 0; i < 3; i++) {
			static const float coefficients[3] = { 5.0f, 2.0f, 5.0f };
			float diff = mRotationTargetVelocities[i] - mRotationVelocities[i];
			float acc = Common::clamp(-1.0f, diff * 0.5f, 1.0f);
			float updatedV = mRotationVelocities[i] + acc * t * coefficients[i];
			mRotationVelocities[i] = Common::clamp(-1.0f, updatedV, 1.0f);
		}
	}

	// update velocity
	mVelocity = Common::Math::rotate3D(Common::Vector3(0, 0, 1), mRotation) * 40.0f;

	VisibleEntity::update(t);

	if(mController)
		mController->update(t);

	if(mTargetUpdateTimer.countdownAndRewind(t)) {
		mTarget = nullptr;
		auto planes = mGame->getPlanes();
		for(auto p : planes) {
			if(p != this) {
				mTarget = p;
				break;
			}
		}
	}

	checkShooting();
}

void Vehicle::setTargetVelocity(PrincipalAxis a, float v)
{
	mRotationTargetVelocities[int(a)] = v;
}

void Vehicle::setController(PlaneController* p)
{
	mController = p;
}

void Vehicle::shoot()
{
	if(!mDestroyed)
		mShooting = true;
}

void Vehicle::checkShooting()
{
	if(mShooting) {
		mShooting = false;
		if(!mMissiles.empty()) {
			auto mit = mMissiles.rbegin();
			(*mit)->shoot(mTarget);
			mMissiles.pop_back();
		}
	}
}

void Vehicle::addMissile(Missile* m)
{
	Common::Vector3 offset(-4.80f, -0.18f, -1.0f);
	switch(mMissiles.size()) {
		case 0:
			break;

		case 1:
			offset.x = -offset.x;
			break;

		default:
			throw std::runtime_error("Plane only supports two missiles");
	}
	m->setOffset(offset);
	mMissiles.push_back(m);
}

void Vehicle::destroy()
{
	mDestroyed = true;
}

bool Vehicle::isDestroyed() const
{
	return mDestroyed;
}

const char* Vehicle::getType() const
{
	return "f16";
}



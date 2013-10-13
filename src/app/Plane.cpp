#include "Plane.h"
#include "Game.h"
#include "Terrain.h"
#include "Missile.h"
#include "common/Math.h"

Plane::Plane(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q)
	: Vehicle(g, side, pos, q)
{
	mBraking = true;
}

bool Plane::acceptsMissiles() const
{
	return mMissiles.size() < 2;
}

void Plane::addMissile(Missile* m)
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

void Plane::update(float t)
{
	if(isDestroyed()) {
		if(!grounded()) {
			Common::Quaternion tgtRot = Common::Quaternion(sqrt(0.5), 0, 0, sqrt(0.5));
			mRotation = mRotation.slerp(tgtRot,
					Common::clamp(0.0f, 0.3f * t, 1.0f));
			mRotationVelocities[0] = 0.0f;
			mRotationVelocities[1] = 0.0f;
			mRotationVelocities[2] = 0.3f;
		} else {
			mVelocity = getVelocity() * 0.99f;
		}
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
	float speed = mVelocity.length();
	float speed_mod = 0.0f;
	if(grounded()) {
		if(!mBraking) {
			speed_mod = 50.0f;
		} else {
			speed_mod = -3.0f;
		}
	} else {
		if(!mBraking) {
			speed_mod = 100.0f;
		} else {
			if(speed > 20.0f)
				speed_mod = -2.0f;
			else
				speed_mod = 10.0f;
		}
	}
	speed = Common::clamp(0.0f, speed + speed_mod * t, 40.0f);
	mVelocity = Common::Math::rotate3D(Common::Vector3(0, 0, 1), mRotation) * speed;

	Vehicle::update(t);
}

const char* Plane::getType() const
{
	return "f16";
}

VehicleType Plane::getVehicleType() const
{
	return VehicleType::Plane;
}



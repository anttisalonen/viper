#include "Plane.h"
#include "Game.h"
#include "Terrain.h"
#include "common/Math.h"

Plane::Plane(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: Vehicle(g, pos, q)
{
	setVelocity(Common::Vector3(0, 0, 10));
}

void Plane::update(float t)
{
	if(isDestroyed()) {
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

	Vehicle::update(t);

	if(grounded()) {
		destroy();
	}
}

const char* Plane::getType() const
{
	return "f16";
}



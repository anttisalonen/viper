#include "SAM.h"
#include "Game.h"
#include "Terrain.h"
#include "common/Math.h"

SAM::SAM(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: Vehicle(g, pos, q)
{
	float height = getGame()->getTerrain()->getHeightAt(pos.x, pos.z);
	setPosition(Common::Vector3(pos.x, height + 6.0f, pos.z));
}

const char* SAM::getType() const
{
	return "sa8";
}

void SAM::update(float t)
{
	// update velocity
	// set the speed along the terrain, but don't change the y component
	// (in case of free fall).
	// http://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis
	float tv = Common::clamp(-0.5f, mTargetVelocity, 1.0f);
	auto oldVel = mVelocity;
	float yaw = getYaw();
	auto oldSpeed = Common::Vector3(mVelocity.x, 0.0f, mVelocity.z).length();
	auto newSpeed = oldSpeed + tv;
	Common::Vector3 rotated = Common::Math::rotate2D(Common::Vector3(0, 1, 0), yaw);
	mVelocity = Common::Vector3(rotated.x, 0.0f, rotated.y) * newSpeed;
	mVelocity.truncate(20.0f); // this sets the top speed
	mVelocity.y = oldVel.y;

	// update yaw
	static const float yawCoefficient = 3.0f;
	float diff = mRotationTargetVelocities[1] - mRotationVelocities[1];
	float acc = Common::clamp(-1.0f, diff * 0.5f, 1.0f);
	float updatedV = mRotationVelocities[1] + acc * t * yawCoefficient;
	mRotationVelocities[1] = Common::clamp(-1.0f, updatedV, 1.0f);

	Vehicle::update(t);
}



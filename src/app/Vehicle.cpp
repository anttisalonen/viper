#include <string.h>
#include <stdexcept>

#include "Vehicle.h"
#include "Missile.h"
#include "Game.h"
#include "Terrain.h"

#include "VehicleController.h"

#include "common/Math.h"

using Common::Quaternion;
using Common::Vector3;

Vehicle::Vehicle(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q)
	: VisibleEntity(pos, q, 10.0f),
	mGame(g),
	mSide(side)
{
	memset(mRotationTargetVelocities, 0x00, sizeof(mRotationTargetVelocities));
}

int Vehicle::getSide() const
{
	return mSide;
}

void Vehicle::update(float t)
{
	VisibleEntity::update(t);

	if(mController)
		mController->update(t);

	if(mTargetUpdateTimer.countdownAndRewind(t)) {
		mTarget = nullptr;
		auto vehicles = mGame->getVehicles();
		for(auto p : vehicles) {
			if(mSide != p->getSide()) {
				mTarget = p;
				break;
			}
		}
	}

	checkShooting();

	/* brute force O(n^2) collision detection between vehicles for now */
	for(auto p2 : mGame->getVehicles()) {
		if(this != p2 && this->collidesWith(*p2)) {
			this->destroy();
			p2->destroy();
		}
	}

	// check whether on ground
	auto pos = getPosition();
	float tHeight = getHeightAt(pos.x, pos.z);
	float minHeight = tHeight;
	mGrounded = minHeight > pos.y;
	if(mGrounded) {
		if(tHeight > pos.y) {
			setPosition(Common::Vector3(pos.x, tHeight, pos.z));
			setRotationToGround();
		}
	} else {
		// gravity
		mVelocity += Common::Vector3(0.0f, -18.0f * t, 0.0f);
	}
}

void Vehicle::setTargetVelocity(PrincipalAxis a, float v)
{
	mRotationTargetVelocities[int(a)] = v;
}

void Vehicle::setTargetVelocity(float v)
{
	mTargetVelocity = v;
}

void Vehicle::setController(VehicleController* p)
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

void Vehicle::destroy()
{
	mDestroyed = true;
}

bool Vehicle::isDestroyed() const
{
	return mDestroyed;
}

const Game* Vehicle::getGame() const
{
	return mGame;
}

Game* Vehicle::getGame()
{
	return mGame;
}

bool Vehicle::grounded() const
{
	return mGrounded;
}

void Vehicle::setRotationToGround()
{
	const auto& orig = getRotation();
	const auto& pos = getPosition();
	Vector3 forward = pos + Common::Math::rotate3D(Vector3(0.0f, 0, 1.0f), orig) * 1.0f;
	Vector3 right = pos + Common::Math::rotate3D(Vector3(-1.0f, 0, 0.0f), orig) * 1.0f;
	float h1 = getHeightAt(pos.x, pos.z);
	float h2 = getHeightAt(forward.x, forward.z);
	float h3 = getHeightAt(right.x, right.z);
	Vector3 p1(pos.x, h1, pos.z);
	Vector3 p2(forward.x, h2, forward.z);
	Vector3 p3(right.x, h3, right.z);
	Vector3 norm = (p2 - p1).normalized().cross((p3 - p1).normalized());

	if(norm.y < 0.0f)
		norm = norm * -1.0f;

	Vector3 dir = Common::Math::rotate3D(Vector3(-1.0f, 0, 0.0f), orig);
	dir.y = 0.0f;
	// rotation along the plane
	Quaternion rot = Quaternion::getRotationTo(Vector3(0, 1, 0), norm);
	// rotation towards heading
	// TODO: this auto-orients the vehicle along the X axis for an unknown reason.
	Quaternion rot2 = Quaternion::getRotationTo(Vector3(-1, 0, 0), dir);
	rot = rot * rot2;
	setRotation(orig.slerp(rot, 0.1f));
}

float Vehicle::getHeightAt(float x, float y) const
{
	return std::max<float>(0.0f, getGame()->getTerrain()->getHeightAt(x, y));
}



#include <string.h>
#include <stdexcept>

#include "Vehicle.h"
#include "Missile.h"
#include "Game.h"
#include "Terrain.h"

#include "PlaneController.h"

#include "common/Math.h"

Vehicle::Vehicle(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: VisibleEntity(pos, q, 10.0f),
	mGame(g)
{
	memset(mRotationTargetVelocities, 0x00, sizeof(mRotationTargetVelocities));
}

void Vehicle::update(float t)
{
	mVelocity += Common::Vector3(0.0f, -18.0f * t, 0.0f);
	VisibleEntity::update(t);

	if(mController)
		mController->update(t);

	if(mTargetUpdateTimer.countdownAndRewind(t)) {
		mTarget = nullptr;
		auto vehicles = mGame->getVehicles();
		for(auto p : vehicles) {
			if(p != this) {
				mTarget = p;
				break;
			}
		}
	}

	checkShooting();

	/* brute force O(n^2) collision detection between planes for now */
	for(auto p2 : mGame->getVehicles()) {
		if(this != p2 && this->collidesWith(*p2)) {
			this->destroy();
			p2->destroy();
		}
	}

	// check whether on ground
	auto pos = getPosition();
	float tHeight = std::max<float>(0.0f, getGame()->getTerrain()->getHeightAt(pos.x, pos.z));
	float minHeight = tHeight + getRadius();
	mGrounded = minHeight > pos.y;
	if(mGrounded) {
		if(tHeight > pos.y) {
			setPosition(Common::Vector3(pos.x, tHeight, pos.z));
		}
	}
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

Game* Vehicle::getGame()
{
	return mGame;
}

bool Vehicle::grounded() const
{
	return mGrounded;
}


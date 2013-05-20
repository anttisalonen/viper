#include <string.h>
#include <stdexcept>

#include "Vehicle.h"
#include "Missile.h"
#include "Game.h"
#include "Terrain.h"

#include "VehicleController.h"

#include "common/Math.h"

Vehicle::Vehicle(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: VisibleEntity(pos, q, 10.0f),
	mGame(g)
{
	memset(mRotationTargetVelocities, 0x00, sizeof(mRotationTargetVelocities));
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
			if(p != this) {
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
			// TODO: this doesn't work correctly ATM
			// setRotationToGround();
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
	// keep yaw, set roll & pitch according to terrain
	const float offs = 3.0f;
	auto pos = getPosition();
	auto posN = pos;  posN.x -= offs;
	auto posS = posN; posS.x += offs;
	auto posW = pos;  posW.z -= offs;
	auto posE = posW; posE.z += offs;

	float heightN = getHeightAt(posN.x, posN.z);
	float heightS = getHeightAt(posS.x, posS.z);
	float heightW = getHeightAt(posW.x, posW.z);
	float heightE = getHeightAt(posE.x, posE.z);

	float hdN = heightN - heightS;
	float hdW = heightW - heightE;
	const float lenN = offs * 2.0f;

	float angleN = atan2(hdN, lenN);
	float angleW = atan2(hdW, lenN);

	float yaw, roll, pitch;
	getRotation().toEuler(pitch, yaw, roll);
	
	if(isnan(yaw)) {
		yaw = 0.0f;
	}

	Common::Vector3 axis(angleN, 1.0f, angleW);
	axis.normalize();
	Common::Quaternion res = Common::Quaternion::fromAxisAngle(axis, yaw);

	Common::Quaternion orig = getRotation();
	orig = orig.slerp(res, 0.1f);
	setRotation(orig);
}

float Vehicle::getHeightAt(float x, float y) const
{
	return std::max<float>(0.0f, getGame()->getTerrain()->getHeightAt(x, y));
}



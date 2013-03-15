#include "Missile.h"
#include "Plane.h"
#include "Game.h"

#include "common/Math.h"

Missile::Missile(const Plane* plane)
	: VisibleEntity(plane->getPosition(), plane->getRotation(), 1.0f),
	mPlane(plane),
	mFuelTime(10.0f)
{
	mVelocity = plane->getVelocity();
}

void Missile::update(float t)
{
	if(mPlane) {
		mRotation = mPlane->getRotation();
		mPosition = mPlane->getPosition() + Common::Math::rotate3D(mOffset, mRotation) * mOffset.length();
		mVelocity = mPlane->getVelocity();
	} else {
		if(mVelocity.length() < 100.0f) {
			mVelocity += mVelocity * 1.5f * t;
		}
		mFuelTime -= t;

		if(mTarget) {
			Common::Quaternion tgtRotation =
				Common::Quaternion::getRotationTo(mVelocity,
						mTarget->getPosition() - getPosition());
			mRotation = mRotation.slerp(tgtRotation, Common::clamp(0.0f, 3.0f * t, 1.0f));
			mVelocity = Common::Math::rotate3D(Common::Vector3(0, 0, 1), mRotation) * mVelocity.length();
		}

		VisibleEntity::update(t);

		/* only check collision against target - very crude for now
		 * (ignore tunneling, spheres as objects) */
		if(collidesWith(*mTarget)) {
			mTarget->destroy();
			mTarget = nullptr;
			mFuelTime = 0.0f;
		}
	}
}

void Missile::shoot(Plane* tgt)
{
	mPlane = nullptr;
	mTarget = tgt;
}

const char* Missile::getType() const
{
	return "sidewinder";
}

void Missile::setOffset(const Common::Vector3& offset)
{
	mOffset = offset;
}

bool Missile::outOfFuel() const
{
	return mFuelTime <= 0.0f;
}

bool Missile::attached() const
{
	return mPlane != nullptr;
}


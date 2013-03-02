#include "Missile.h"
#include "Plane.h"

#include "common/Math.h"

Missile::Missile(const Plane* plane)
	: VisibleEntity(plane->getPosition(), plane->getRotation()),
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
			mVelocity += mVelocity * 0.5f * t;
		}
		mFuelTime -= t;
		VisibleEntity::update(t);
	}
}

void Missile::shoot()
{
	mPlane = nullptr;
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
	return mFuelTime < 0.0f;
}

bool Missile::attached() const
{
	return mPlane != nullptr;
}


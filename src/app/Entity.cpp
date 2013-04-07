#include <string.h>

#include "Entity.h"

VisibleEntity::VisibleEntity(const Common::Vector3& v,
		const Common::Quaternion& q, float radius)
	: mPosition(v),
	mRotation(q),
	mRadius(radius)
{
	memset(mRotationVelocities, 0x00, sizeof(mRotationVelocities));
}

const Common::Vector3& VisibleEntity::getPosition() const
{
	return mPosition;
}

const Common::Quaternion& VisibleEntity::getRotation() const
{
	return mRotation;
}

const Common::Vector3& VisibleEntity::getVelocity() const
{
	return mVelocity;
}

void VisibleEntity::setPosition(const Common::Vector3& v)
{
	mPosition = v;
}

void VisibleEntity::update(float t)
{
	// update position
	mPosition += mVelocity * t;

	// update rotation
	Common::Quaternion rot(0, 0, 0, 1);
	for(int i = 0; i < 3; i++) {
		if(mRotationVelocities[i]) {
			Common::Vector3 axis(i == 0 ? 1 : 0, i == 1 ? 1 : 0, i == 2 ? 1 : 0);
			Common::Quaternion q = Common::Quaternion::fromAxisAngle(axis,
					mRotationVelocities[i] * t);
			rot = rot * q;
		}
	}
	mRotation = mRotation * rot;
	mRotation = mRotation.versor();
}

float VisibleEntity::getRadius() const
{
	return mRadius;
}

bool VisibleEntity::collidesWith(const VisibleEntity& e) const
{
	float dist2 = (getPosition() - e.getPosition()).length2();
	float rr = e.getRadius() + getRadius();
	rr = rr * rr;
	if(rr > dist2) {
		return true;
	} else {
		return false;
	}
}



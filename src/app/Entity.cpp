#include <string.h>

#include "common/Math.h"

#include "Entity.h"

PhysicalEntity::PhysicalEntity(const Common::Vector3& v,
		const Common::Quaternion& q, float radius)
	: mPosition(v),
	mRotation(q),
	mRadius(radius)
{
	memset(mRotationVelocities, 0x00, sizeof(mRotationVelocities));
}

const Common::Vector3& PhysicalEntity::getPosition() const
{
	return mPosition;
}

const Common::Quaternion& PhysicalEntity::getRotation() const
{
	return mRotation;
}

const Common::Vector3& PhysicalEntity::getVelocity() const
{
	return mVelocity;
}

void PhysicalEntity::setPosition(const Common::Vector3& v)
{
	mPosition = v;
}

void PhysicalEntity::setVelocity(const Common::Vector3& v)
{
	mVelocity = v;
}

void PhysicalEntity::setRotation(const Common::Quaternion& q)
{
	mRotation = q;
}

void PhysicalEntity::update(float t)
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

float PhysicalEntity::getRadius() const
{
	return mRadius;
}

bool PhysicalEntity::collidesWith(const PhysicalEntity& e) const
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

float PhysicalEntity::getYaw() const
{
	Common::Vector3 ortho = Common::Vector3(1, 0, 0);
	Common::Vector3 rot = Common::Math::rotate3D(ortho, mRotation);
	// project rot to xz plane
	rot.y = 0.0f;
	rot.normalize();
	float yaw = acos(rot.dot(ortho));
	if(rot.z < 0.0f)
		yaw = -yaw;
	return yaw;
}

VisibleEntity::VisibleEntity(const Common::Vector3& v,
		const Common::Quaternion& q, float radius)
	: PhysicalEntity(v, q, radius)
{
}



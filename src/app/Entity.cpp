#include "Entity.h"

VisibleEntity::VisibleEntity(const Common::Vector3& v,
		const Common::Quaternion& q)
	: mPosition(v),
	mRotation(q)
{
}

const Common::Vector3& VisibleEntity::getPosition() const
{
	return mPosition;
}

const Common::Quaternion& VisibleEntity::getRotation() const
{
	return mRotation;
}



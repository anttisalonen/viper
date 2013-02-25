#include "Plane.h"

Plane::Plane(const Common::Vector3& pos)
	: VisibleEntity(pos, Common::Quaternion(1, 0, 0, 0))
{
}

void Plane::update(float t)
{
}

void Plane::setTargetPitchVelocity(float v)
{
}



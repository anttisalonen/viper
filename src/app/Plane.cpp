#include "Plane.h"

#include "PlaneController.h"

Plane::Plane(const Common::Vector3& pos)
	: VisibleEntity(pos, Common::Quaternion(1, 0, 0, 0))
{
}

void Plane::update(float t)
{
	if(mController)
		mController->update(t);
}

void Plane::setTargetPitchVelocity(float v)
{
}

void Plane::setController(PlaneController* p)
{
	mController = p;
}


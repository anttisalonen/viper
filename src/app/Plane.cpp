#include <string.h>

#include "common/Math.h"

#include "Plane.h"

#include "PlaneController.h"

Plane::Plane(const Common::Vector3& pos)
	: VisibleEntity(pos, Common::Quaternion(0, 0, 0, 1))
{
	memset(mRotationVelocities, 0x00, sizeof(mRotationVelocities));
	memset(mRotationTargetVelocities, 0x00, sizeof(mRotationTargetVelocities));
	mVelocity = Common::Vector3(0, 0, 10);
}

void Plane::update(float t)
{
	// update position
	mPosition += mVelocity * t;

	// update rotation
	Common::Quaternion rot(0, 0, 0, 1);
	for(int i = 0; i < 3; i++) {
		float diff = mRotationTargetVelocities[i] - mRotationVelocities[i];
		float acc = Common::clamp(-1.0f, diff * 0.5f, 1.0f);
		float updatedV = mRotationVelocities[i] + acc * t;
		mRotationVelocities[i] = Common::clamp(-1.0f, updatedV, 1.0f);

		if(mRotationVelocities[i]) {
			static const float coefficient[3] = { 5.0f, 2.0f, 5.0f };

			Common::Vector3 axis(i == 0 ? 1 : 0, i == 1 ? 1 : 0, i == 2 ? 1 : 0);
			Common::Quaternion q = Common::Quaternion::fromAxisAngle(axis,
					mRotationVelocities[i] * t * coefficient[i]);
			rot = rot * q;
		}
	}
	mRotation = mRotation * rot;
	mRotation = mRotation.versor();

	if(mController)
		mController->update(t);

	mVelocity = Common::Math::rotate3D(Common::Vector3(0, 0, 1), mRotation) * 40.0f;
}

void Plane::setTargetVelocity(PrincipalAxis a, float v)
{
	mRotationTargetVelocities[int(a)] = v;
}

void Plane::setController(PlaneController* p)
{
	mController = p;
}

const Common::Vector3& Plane::getVelocity() const
{
	return mVelocity;
}



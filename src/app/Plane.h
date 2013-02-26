#ifndef APP_PLANE_H
#define APP_PLANE_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Entity.h"

class PlaneController;

enum class PrincipalAxis {
	Pitch,
	Yaw,
	Roll,
};

class Plane : public VisibleEntity {
	public:
		Plane(const Common::Vector3& pos);
		void update(float t);
		void setTargetVelocity(PrincipalAxis a, float v);
		void setController(PlaneController* p);
		const Common::Vector3& getVelocity() const;

	private:
		PlaneController* mController = nullptr;
		Common::Vector3 mVelocity;
		float mRotationVelocities[3];
		float mRotationTargetVelocities[3];
};

#endif


#ifndef APP_PLANE_H
#define APP_PLANE_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Entity.h"

class Plane : public VisibleEntity {
	public:
		Plane(const Common::Vector3& pos);
		void update(float t);
		void setTargetPitchVelocity(float v);
};

#endif


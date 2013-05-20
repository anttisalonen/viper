#ifndef APP_PLANE_H
#define APP_PLANE_H

#include "Vehicle.h"

class Plane : public Vehicle {
	public:
		Plane(Game* g, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
		virtual const char* getType() const override;
};

#endif


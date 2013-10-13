#ifndef APP_LANDVEHICLE_H
#define APP_LANDVEHICLE_H

#include "Vehicle.h"

class LandVehicle : public Vehicle {
	public:
		LandVehicle(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
};

#endif


#ifndef APP_TANK_H
#define APP_TANK_H

#include "LandVehicle.h"

class Tank : public LandVehicle {
	public:
		Tank(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual const char* getType() const override;
		virtual VehicleType getVehicleType() const override;
};

#endif


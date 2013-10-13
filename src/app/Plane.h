#ifndef APP_PLANE_H
#define APP_PLANE_H

#include "Vehicle.h"

class Plane : public Vehicle {
	public:
		Plane(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
		virtual const char* getType() const override;
		virtual bool acceptsMissiles() const override;
		virtual void addMissile(Missile* m) override;
		virtual VehicleType getVehicleType() const override;
};

#endif


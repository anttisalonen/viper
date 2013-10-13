#ifndef APP_SAM_H
#define APP_SAM_H

#include "LandVehicle.h"

class SAM : public LandVehicle {
	public:
		SAM(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual const char* getType() const override;
		virtual bool acceptsMissiles() const override;
		virtual void addMissile(Missile* m) override;
		virtual VehicleType getVehicleType() const override;
};

#endif


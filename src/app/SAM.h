#ifndef APP_SAM_H
#define APP_SAM_H

#include "Vehicle.h"

class SAM : public Vehicle {
	public:
		SAM(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q);
		virtual void update(float t) override;
		virtual const char* getType() const override;
		virtual void addMissile(Missile* m) override;
};

#endif


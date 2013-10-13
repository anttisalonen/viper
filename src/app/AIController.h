#ifndef APP_AICONTROLLER_H
#define APP_AICONTROLLER_H

#include "VehicleController.h"

class AIController : public VehicleController {
	public:
		AIController(int side, Vehicle* p = nullptr);
		virtual void update(float t) override;
		int getSide() const override;

	private:
		int mSide = 0;
};

#endif


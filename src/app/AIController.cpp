#include "AIController.h"

AIController::AIController(int side, Vehicle* p)
	: VehicleController(p),
	mSide(side)
{
}

int AIController::getSide() const
{
	return mSide;
}

void AIController::update(float t)
{
	if(mVehicle) {
		//mVehicle->setTargetVelocity(100.0f);
	}
}


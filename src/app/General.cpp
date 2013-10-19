#include "General.h"
#include "Game.h"
#include "Vehicle.h"

General::General(Game* g)
	: mGame(g)
{
}

const Vehicle* General::getSelectedVehicle() const
{
	return mSelectedVehicle;
}

void General::setSelectedVehicle(Vehicle* v)
{
	mSelectedVehicle = v;
}

void General::update(float time)
{
	return;
}



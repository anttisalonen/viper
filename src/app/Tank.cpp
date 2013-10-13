#include "Tank.h"
#include "Game.h"
#include "Terrain.h"
#include "Missile.h"
#include "common/Math.h"

Tank::Tank(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q)
	: LandVehicle(g, side, pos, q)
{
}

const char* Tank::getType() const
{
	return "sa8";
}

VehicleType Tank::getVehicleType() const
{
	return VehicleType::Tank;
}



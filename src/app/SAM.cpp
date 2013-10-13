#include "SAM.h"
#include "Game.h"
#include "Terrain.h"
#include "Missile.h"
#include "common/Math.h"

SAM::SAM(Game* g, int side, const Common::Vector3& pos, const Common::Quaternion& q)
	: LandVehicle(g, side, pos, q)
{
}

bool SAM::acceptsMissiles() const
{
	return mMissiles.size() < 4;
}

void SAM::addMissile(Missile* m)
{
	Common::Vector3 offset(-1.80f, 0.18f, 1.0f);
	offset.x += 0.5f * mMissiles.size();
	if(mMissiles.size() == 4)
		throw std::runtime_error("SAM only supports four missiles");
	m->setOffset(offset);
	mMissiles.push_back(m);
}

const char* SAM::getType() const
{
	return "sa8";
}

VehicleType SAM::getVehicleType() const
{
	return VehicleType::SAM;
}



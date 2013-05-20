#include "SAM.h"
#include "Game.h"
#include "Terrain.h"

SAM::SAM(Game* g, const Common::Vector3& pos, const Common::Quaternion& q)
	: Vehicle(g, pos, q)
{
	float height = getGame()->getTerrain()->getHeightAt(pos.x, pos.z);
	setPosition(Common::Vector3(pos.x, height + 6.0f, pos.z));
}

const char* SAM::getType() const
{
	return "sa8";
}



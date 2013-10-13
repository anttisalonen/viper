#include "Game.h"
#include "InputHandler.h"
#include "UserInterface.h"

#include "Terrain.h"
#include "Plane.h"
#include "Missile.h"
#include "SAM.h"

#include "common/Math.h"

Game::Game()
	: mInputHandler(new InputHandler())
{
	using Common::Vector3;
	Constants terrainConstants("share/terrain/constants.json");
	float scale = terrainConstants.getFloat("height_scale");
	float offset = terrainConstants.getFloat("height_offset");
	unsigned int dim = terrainConstants.getUInt("dimension");
	mTerrain = new Terrain(scale, offset, dim);

	Vector3 base0(dim * 0.5 - 100, 0, dim * 0.5 - 100);
	Vector3 base1(100 - dim * 0.5, 0, 100 - dim * 0.5);

	float base_len = 300.0f;
	float base_width = 50.0f;

	Vector3 base0_min = base0 + Vector3(-base_len, 0, -base_width);
	Vector3 base0_max = base0 + Vector3(50, 0, 50);
	Vector3 base1_min = base1 + Vector3(-50, 0, -50);
	Vector3 base1_max = base1 + Vector3(base_width, 0, base_len);

	mBaseFlags[0] = base0_max + Vector3(-10, 0, -10); mBaseFlags[0].y = mTerrain->getHeightAt(mBaseFlags[0].x, mBaseFlags[0].z);
	mBaseFlags[1] = base1_min + Vector3(10, 0, 10);   mBaseFlags[1].y = mTerrain->getHeightAt(mBaseFlags[1].x, mBaseFlags[1].z);

	float base0_height = mTerrain->getHeightAt(base0_min.x + base_len * 0.5f, base0_min.z);
	float base1_height = mTerrain->getHeightAt(base1_max.x, base1_max.z - base_len * 0.5f);

	assert(base0_height > 5.0f);
	assert(base1_height > 5.0f);

	mTerrain->addHeightModifier(base0_min.x, base0_min.z, base0_max.x, base0_max.z, base0_height);
	mTerrain->addHeightModifier(base1_min.x, base1_min.z, base1_max.x, base1_max.z, base1_height);

	mUserInterface = new UserInterface(mInputHandler, mTerrain);

	Common::Quaternion q0 = Common::Quaternion::getRotationTo(Vector3(0, 0, 1), Vector3(-1, 0, 0));
	Common::Quaternion q1 = Common::Quaternion::getRotationTo(Vector3(0, 0, 1), Vector3(0, 0, 1));

	Plane* p = addPlane(0, base0 - Vector3(0, 0, 30), q0);
	p->setController(mInputHandler);
	mInputHandler->setVehicle(p);
	mTrackingVehicle = p;

	addSAM(0, base0, q0);

	addPlane(1, base1 + Vector3(30, 0, 0), q1);
	addSAM(1, base1, q1);
}

Game::~Game()
{
	mTrackingVehicle = nullptr;
	for(auto p : mVehicles) {
		delete p;
	}
	mVehicles.clear();

	for(auto p : mMissiles) {
		delete p;
	}
	mMissiles.clear();

	delete mInputHandler;
	mInputHandler = nullptr;
	delete mUserInterface;
	mUserInterface = nullptr;
	delete mTerrain;
	mTerrain = nullptr;
}

void Game::go()
{
	double prevTime = Common::Clock::getTime();
	while(!mUserInterface->isClosed()) {
		mUserInterface->renderOneFrame();
		double thisTime = Common::Clock::getTime();
		double diffTime = thisTime - prevTime;
		prevTime = thisTime;
		if(!update(diffTime)) {
			break;
		}
		mFPSTimer.limitFPS(60, false);
	}
}

Plane* Game::addPlane(int side, const Common::Vector3& pos, const Common::Quaternion& q)
{
	Plane* p = new Plane(this, side, pos, q);
	mVehicles.push_back(p);
	for(unsigned int i = 0; i < 2; i++) {
		Missile* m = new Missile(p);
		p->addMissile(m);
		mMissiles.push_back(m);
	}
	return p;
}

SAM* Game::addSAM(int side, const Common::Vector3& pos, const Common::Quaternion& q)
{
	SAM* s = new SAM(this, side, pos, q);
	mVehicles.push_back(s);
	for(unsigned int i = 0; i < 4; i++) {
		Missile* m = new Missile(s);
		s->addMissile(m);
		mMissiles.push_back(m);
	}
	return s;
}

bool Game::update(float frameTime)
{
	for(auto p : mVehicles) {
		p->update(frameTime);
		mUserInterface->updateVehicle(p);
		if(mWinner == -1 && !p->isDestroyed() && p->grounded()) {
			if(p->getSide() == 0 && p->getPosition().distance(mBaseFlags[1]) < 20.0f) {
				mWinner = 0;
				std::cout << "0 won!\n";
			} else if(p->getSide() == 1 && p->getPosition().distance(mBaseFlags[0]) < 20.0f) {
				mWinner = 1;
				std::cout << "1 won!\n";
			}
		}
	}

	for(auto mit = mMissiles.begin(); mit != mMissiles.end(); ) {
		(*mit)->update(frameTime);
		mUserInterface->updateMissile(*mit);

		if((*mit)->outOfFuel()) {
			mUserInterface->removeMissile(*mit);
			delete *mit;
			mit = mMissiles.erase(mit);
		} else {
			++mit;
		}
	}

	if(mInputHandler->checkVehicleChangeRequest()) {
		auto it = mVehicles.begin();
		for(; it != mVehicles.end(); ++it) {
			if(*it == mInputHandler->getVehicle()) {
				break;
			}
		}
		if(it != mVehicles.end()) {
			do {
				++it;
				if(it == mVehicles.end()) {
					it = mVehicles.begin();
				}
				if((*it)->getSide() == 0 && !(*it)->isDestroyed()) {
					(*it)->setController(mInputHandler);
					mInputHandler->setVehicle(*it);
					mTrackingVehicle = *it;
					break;
				}
			} while(*it != mInputHandler->getVehicle());
		}
	}

	if(mTrackingVehicle) {
		switch(mInputHandler->getCurrentViewSetting()) {
			case ViewSetting::Cockpit:
				{
					Common::Vector3 viewpoint = mTrackingVehicle->getPosition() +
						Common::Math::rotate3D(Common::Vector3(0, 2, 5),
								mTrackingVehicle->getRotation()) * 5.0f;
					mUserInterface->setCamera(viewpoint, mTrackingVehicle->getRotation());
				}
				break;

			case ViewSetting::Offset:
				{
					Common::Vector3 offset = Common::Vector3(0, 5, -30);
					mUserInterface->setCamera(offset, mTrackingVehicle->getPosition(),
							mTrackingVehicle->getRotation() *
							mInputHandler->getViewRotation());
				}
				break;
		}
	}

	return mInputHandler->running();
}

std::list<Vehicle*>& Game::getVehicles()
{
	return mVehicles;
}

const Terrain* Game::getTerrain() const
{
	return mTerrain;
}


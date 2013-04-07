#include "Game.h"
#include "InputHandler.h"
#include "UserInterface.h"

#include "Terrain.h"
#include "Plane.h"
#include "Missile.h"

#include "common/Math.h"

Game::Game()
	: mInputHandler(new InputHandler())
{
	Constants terrainConstants("share/terrain/constants.json");
	float scale = terrainConstants.getFloat("height_scale");
	float offset = terrainConstants.getFloat("height_offset");
	unsigned int dim = terrainConstants.getUInt("dimension");
	mTerrain = new Terrain(scale, offset, dim);

	mUserInterface = new UserInterface(mInputHandler, mTerrain);

	Plane* p = addPlane(Common::Vector3(200, 250, 50), Common::Quaternion(0, 0, 0, 1));
	p->setController(mInputHandler);
	mInputHandler->setPlane(p);

	mTrackingVehicle = p;

	addPlane(Common::Vector3(200, 280, 460), Common::Quaternion(0, sqrt(2.0f) * 0.5f, 0, sqrt(2.0f) * 0.5f));
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

Plane* Game::addPlane(const Common::Vector3& pos, const Common::Quaternion& q)
{
	Plane* p = new Plane(this, pos, q);
	mVehicles.push_back(p);
	for(unsigned int i = 0; i < 2; i++) {
		Missile* m = new Missile(p);
		p->addMissile(m);
		mMissiles.push_back(m);
	}
	return p;
}

bool Game::update(float frameTime)
{
	for(auto p : mVehicles) {
		p->update(frameTime);
		mUserInterface->updateVehicle(p);

		/* brute force O(n^2) collision detection between planes for now */
		for(auto p2 : mVehicles) {
			if(p != p2 && p->collidesWith(*p2)) {
				p->destroy();
				p2->destroy();
			}
		}

		auto pos = p->getPosition();
		float tHeight = std::max<float>(0.0f, mTerrain->getHeightAt(pos.x, pos.z));
		float minHeight = tHeight + p->getRadius();
		if(minHeight > pos.y) {
			p->destroy();
			if(tHeight > pos.y) {
				p->setPosition(Common::Vector3(pos.x, tHeight, pos.z));
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


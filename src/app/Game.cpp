#include "Game.h"
#include "InputHandler.h"
#include "UserInput.h"
#include "GeneralInput.h"
#include "UserInterface.h"

#include "Terrain.h"
#include "Plane.h"
#include "Missile.h"
#include "SAM.h"
#include "Tank.h"
#include "General.h"

#include "AIController.h"

#include "common/Math.h"

Game::Game()
	: mUserInput(new UserInput()),
	mGeneralInput(new GeneralInput()),
	mInputHandler(new InputHandler(mUserInput, mGeneralInput))
{
	using Common::Vector3;
	Constants terrainConstants("share/terrain/constants.json");
	float scale = terrainConstants.getFloat("height_scale");
	float offset = terrainConstants.getFloat("height_offset");
	unsigned int dim = terrainConstants.getUInt("dimension");
	mTerrain = new Terrain(scale, offset, dim);

	mGenerals[0] = new General(this);
	mGenerals[1] = new General(this);

	mGeneralInput->setGeneral(mGenerals[0]);

	mBase[0] = Vector3(dim * 0.5 - 160, 0, dim * 0.5 - 160); mBase[0].y = mTerrain->getHeightAt(mBase[0].x, mBase[0].z);
	mBase[1] = Vector3(160 - dim * 0.5, 0, 160 - dim * 0.5); mBase[1].y = mTerrain->getHeightAt(mBase[1].x, mBase[1].z);

	mBaseLength = 300.0f;
	mBaseWidth = 50.0f;

	Vector3 base0_min = mBase[0] + Vector3(-mBaseLength, 0, -mBaseWidth);
	Vector3 base0_max = mBase[0] + Vector3(50, 0, 50);
	Vector3 base1_min = mBase[1] + Vector3(-50, 0, -50);
	Vector3 base1_max = mBase[1] + Vector3(mBaseWidth, 0, mBaseLength);

	float base0_height = mTerrain->getHeightAt(base0_min.x + mBaseLength * 0.5f, base0_min.z);
	float base1_height = mTerrain->getHeightAt(base1_max.x, base1_max.z - mBaseLength * 0.5f);

	assert(base0_height > 5.0f);
	assert(base1_height > 5.0f);

	mTerrain->addHeightModifier(base0_min.x, base0_min.z, base0_max.x, base0_max.z, base0_height);
	mTerrain->addHeightModifier(base1_min.x, base1_min.z, base1_max.x, base1_max.z, base1_height);

	auto gen_campos = mBase[0];
	gen_campos.y = 500.0f;
	mGeneralInput->setCameraPosition(gen_campos);

	mUserInterface = new UserInterface(mInputHandler, mTerrain);

	srand(21);

	Vehicle* p = addVehicle(VehicleType::Plane, 0);
	assignController(mInputHandler, p);
	mTrackingVehicle = p;

	addVehicle(VehicleType::SAM, 0);

	addVehicle(VehicleType::Plane, 1);
	addVehicle(VehicleType::SAM, 1);

	for(int s = 0; s < 2; s++) {
		for(int i = 0; i < 5; i++) {
			addVehicle(VehicleType::Tank, s);
		}
	}
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
	delete mGeneralInput;
	mGeneralInput = nullptr;
	delete mUserInput;
	mUserInput = nullptr;
	delete mUserInterface;
	mUserInterface = nullptr;
	delete mTerrain;
	mTerrain = nullptr;
}

/* return true if a controller was replaced */
bool Game::assignController(VehicleController* c, Vehicle* v)
{
	auto oldvehicle = c->getVehicle();
	if(oldvehicle)
		oldvehicle->setController(nullptr);

	auto oldcontroller = v->setController(c);
	c->setVehicle(v);
	if(oldcontroller) {
		oldcontroller->setVehicle(nullptr);
		tryAssignController(oldcontroller);
		return true;
	}
	return false;
}

/* return true if the controller could be assigned somewhere */
bool Game::tryAssignController(VehicleController* c)
{
	for(auto v : mVehicles) {
		if(v->getSide() == c->getSide() && v->getController() == nullptr) {
			bool ret = assignController(c, v);
			assert(ret == false);
			return true;
		}
	}
	return false;
}

/* return true if an AI controller could be assigned to the vehicle */
bool Game::tryAssignVehicle(Vehicle* v)
{
	assert(v->getController() == nullptr);
	for(auto c : mAIControllers) {
		if(v->getSide() == c->getSide() && c->getVehicle() == nullptr) {
			bool ret = assignController(c, v);
			assert(ret == false);
			return true;
		}
	}
	return false;
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

Vehicle* Game::addVehicle(VehicleType t, int side)
{
	Vehicle* v = nullptr;
	using Common::Vector3;
	Vector3 pos;
	Common::Quaternion q;

	if(side == 0) {
		pos = mBase[0] - Vector3(rand() % (int)mBaseLength, 0, rand() % (int)mBaseWidth);
		q = Common::Quaternion::getRotationTo(Vector3(0, 0, 1), Vector3(-1, 0, 0));
	} else {
		pos = mBase[1] + Vector3(rand() % (int)mBaseWidth, 0, rand() % (int)mBaseLength);
		q = Common::Quaternion::getRotationTo(Vector3(0, 0, 1), Vector3(0, 0, 1));
	}

	switch(t) {
		case VehicleType::Plane:
			v = new Plane(this, side, pos, q);
			break;

		case VehicleType::SAM:
			v = new SAM(this, side, pos, q);
			break;

		case VehicleType::Tank:
			v = new Tank(this, side, pos, q);
			break;
	}

	assert(v);
	mVehicles.push_back(v);
	while(v->acceptsMissiles()) {
		Missile* m = new Missile(v);
		v->addMissile(m);
		mMissiles.push_back(m);
	}

	VehicleController* ai = new AIController(side);
	assignController(ai, v);
	mAIControllers.push_back(ai);

	return v;
}

bool Game::update(float frameTime)
{
	for(auto p : mVehicles) {
		p->update(frameTime);
		mUserInterface->updateVehicle(p);
		if(mWinner == -1 && !p->isDestroyed() && p->grounded()) {
			if(p->getSide() == 0 && p->getPosition().distance(mBase[1]) < 20.0f) {
				mWinner = 0;
				std::cout << "0 won!\n";
			} else if(p->getSide() == 1 && p->getPosition().distance(mBase[0]) < 20.0f) {
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

	if(mGeneralInput->checkGeneralToggle()) {
		mInputHandler->setGeneralMode(false);
		tryAssignController(mInputHandler);
		mTrackingVehicle = mInputHandler->getVehicle();
	}

	if(mInputHandler->checkGeneralToggle()) {
		if(mTrackingVehicle) {
			mInputHandler->setVehicle(nullptr);
			mTrackingVehicle->setController(nullptr);
			tryAssignVehicle(mTrackingVehicle);
			mTrackingVehicle = nullptr;
		} else {
			tryAssignController(mInputHandler);
			mTrackingVehicle = mInputHandler->getVehicle();
		}
	}

	if(mInputHandler->checkVehicleChangeRequest()) {
		mUserInterface->setMouseVisible(false);
		auto it = mVehicles.begin();
		if(mInputHandler->getVehicle()) {
			for(; it != mVehicles.end(); ++it) {
				if(*it == mInputHandler->getVehicle()) {
					break;
				}
			}
		}

		if(it != mVehicles.end()) {
			do {
				++it;
				if(it == mVehicles.end()) {
					it = mVehicles.begin();
				}
				if((*it)->getSide() == 0 && !(*it)->isDestroyed()) {
					assignController(mInputHandler, *it);
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
					auto vr = mInputHandler->getViewRotation();
					mUserInterface->setCamera(viewpoint, mTrackingVehicle->getRotation() * vr);
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
		mTrackingVehicle->setTurretRotation(mInputHandler->getViewRotation());
	} else {
		// general input
		mGeneralInput->update(frameTime);
		auto campos = mGeneralInput->getCameraPosition();
		Common::Vector3 offset = Common::Vector3(0, campos.y, 0);
		campos.y = 0.0f;
		mUserInterface->setCamera(offset, campos,
				mGeneralInput->getCameraRotation());
		mUserInterface->setMouseVisible(true);
		float x, y;
		if(mGeneralInput->mouseClicked(x, y)) {
			auto oldv = mGenerals[0]->getSelectedVehicle();
			if(oldv) {
				mUserInterface->setEntityHighlight(oldv, false);
			}
			auto v = selectVehicleAt(x, y, 0);
			mGenerals[0]->setSelectedVehicle(v);
			if(v) {
				mUserInterface->setEntityHighlight(v, true);
			}
		}
	}

	return mUserInput->running();
}

std::list<Vehicle*>& Game::getVehicles()
{
	return mVehicles;
}

const Terrain* Game::getTerrain() const
{
	return mTerrain;
}

Vehicle* Game::selectVehicleAt(float x, float y, int side)
{
	Common::Vector3 origin, dir;
	mUserInterface->raycast(x, y, origin, dir);
	auto v = Vehicle::pickVehicle(mVehicles, nullptr, origin, origin + dir * 100.0f, 5.0f);
	if(v && v->getSide() == side)
		return v;
	return nullptr;
}


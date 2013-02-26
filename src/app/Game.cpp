#include "Game.h"
#include "InputHandler.h"
#include "App.h"

Game::Game(App* app, InputHandler* ih)
	: mApp(app),
	mInputHandler(ih)
{
	Plane* p = new Plane(Common::Vector3(20, 25, 50));
	mPlanes.push_back(p);
	p->setController(mInputHandler);
	mInputHandler->setPlane(p);

	mTrackingPlane = p;

	Plane* p2 = new Plane(Common::Vector3(20, 55, 60));
	mPlanes.push_back(p2);
}

Game::~Game()
{
	mTrackingPlane = nullptr;
	for(auto p : mPlanes) {
		delete p;
	}
	mPlanes.clear();
}

bool Game::update(float frameTime)
{
	if(mTrackingPlane) {
		Common::Vector3 offset = Common::Vector3(0, 0, -30);
		offset.y += 5.0f;
		mApp->setCamera(offset, mTrackingPlane->getPosition(),
				mTrackingPlane->getRotation());
	}

	for(auto p : mPlanes) {
		p->update(frameTime);
		mApp->updatePlane(p, p->getPosition(), p->getRotation());
	}
	return mInputHandler->running();
}


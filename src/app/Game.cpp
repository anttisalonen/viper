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
		Common::Vector3 offset = mTrackingPlane->getVelocity() * -3.0f;
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


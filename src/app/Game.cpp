#include "Game.h"
#include "InputHandler.h"
#include "App.h"

Game::Game(App* app, InputHandler* ih)
	: mApp(app),
	mInputHandler(ih)
{
	Plane* p = new Plane(Common::Vector3(20, 25, 50));
	mPlanes.push_back(p);
}

Game::~Game()
{
	for(auto p : mPlanes) {
		delete p;
	}
	mPlanes.clear();
}

bool Game::update(float frameTime)
{
	for(auto p : mPlanes) {
		mApp->updatePlane(p, p->getPosition(), p->getRotation());
	}
	return mInputHandler->update(frameTime);
}


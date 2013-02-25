#include "InputHandler.h"

#include "common/Math.h"

InputHandler::InputHandler()
{
}

bool InputHandler::update(float time)
{
	if(mForward) {
		mForward *= 1.5f;
		mForward = Common::clamp(-1.0f, mForward, 1.0f);
	}
	//mApp->getPlaneNode()->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90.0f * time));
	return mRunning;
}

bool InputHandler::keyPressed(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_ESCAPE:
			mRunning = false;
			break;

		case OIS::KC_UP:
		case OIS::KC_W:
			mForward = 0.1f;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			mForward = -0.1f;
			break;

		default:
			break;
	}

	return true;
}

bool InputHandler::keyReleased(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mForward = 0.0f;
			break;

		default:
			break;
	}

	return true;
}

bool InputHandler::mouseMoved(const OIS::MouseEvent& arg)
{
	return true;
}

bool InputHandler::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}

bool InputHandler::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}



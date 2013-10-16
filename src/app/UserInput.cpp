#include "UserInput.h"

bool UserInput::running() const
{
	return mRunning;
}

bool UserInput::keyPressed(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_ESCAPE:
			mRunning = false;
			break;

		default:
			break;
	}

	return true;
}

bool UserInput::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool UserInput::mouseMoved(const OIS::MouseEvent& arg)
{
	return true;
}

bool UserInput::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}

bool UserInput::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}



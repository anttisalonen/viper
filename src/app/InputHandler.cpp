#include "InputHandler.h"

InputHandler::InputHandler()
{
}

bool InputHandler::running() const
{
	return mRunning;
}

void InputHandler::planeReset()
{
}

void InputHandler::update(float time)
{
	mPitch *= 1.5f;
	if(mPlane) {
		mPlane->setTargetVelocity(PrincipalAxis::Pitch, mPitch);
		mPlane->setTargetVelocity(PrincipalAxis::Yaw, mYaw);
		mPlane->setTargetVelocity(PrincipalAxis::Roll, mRoll);
	}
}

bool InputHandler::keyPressed(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_ESCAPE:
			mRunning = false;
			break;

		case OIS::KC_UP:
		case OIS::KC_W:
			mPitch = 0.1f;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			mPitch = -0.1f;
			break;

		case OIS::KC_Q:
			mYaw = 0.1f;
			break;

		case OIS::KC_E:
			mYaw = -0.1f;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			mRoll = -0.1f;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mRoll = 0.1f;
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
		case OIS::KC_DOWN:
		case OIS::KC_W:
		case OIS::KC_S:
			mPitch = 0.0f;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_RIGHT:
		case OIS::KC_A:
		case OIS::KC_D:
			mRoll = 0.0f;
			break;

		case OIS::KC_Q:
		case OIS::KC_E:
			mYaw = 0.0f;
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



#include "InputHandler.h"
#include "common/Math.h"

#include <string.h>

InputHandler::InputHandler()
{
}

bool InputHandler::running() const
{
	return mRunning;
}

void InputHandler::update(float time)
{
	mPitch = Common::clamp(-1.0f, mPitch * 1.1f, 1.0f);
	mYaw   = Common::clamp(-1.0f, mYaw   * 1.1f, 1.0f);
	mRoll  = Common::clamp(-1.0f, mRoll  * 1.1f, 1.0f);

	mAcceleration = Common::clamp(-1.0f, mAcceleration * 1.1f, 1.0f);

	if(mVehicle) {
		mVehicle->setTargetVelocity(PrincipalAxis::Pitch, mPitch);
		mVehicle->setTargetVelocity(PrincipalAxis::Yaw, mYaw);
		mVehicle->setTargetVelocity(PrincipalAxis::Roll, mRoll);
		mVehicle->setTargetVelocity(mAcceleration);

		if(mShooting) {
			mShooting = false;
			mVehicle->shoot();
		}
	}
}

void InputHandler::printInfo()
{
	if(mVehicle) {
		std::cout << "Position: " << mVehicle->getPosition() << "\n";
		std::cout << "Velocity: " << mVehicle->getVelocity() << "\n";
		std::cout << "Rotation: " << mVehicle->getRotation() << "\n";
		float yaw, roll, pitch;
		mVehicle->getRotation().toEuler(pitch, yaw, roll);
		std::cout << "Pitch/Yaw/Roll: " << pitch << " / " << yaw << " / " << roll << "\n";
	}
}

void InputHandler::requestVehicleChange()
{
	mRequestingVehicleChange = true;
}

bool InputHandler::checkVehicleChangeRequest()
{
	if(mRequestingVehicleChange) {
		mRequestingVehicleChange = false;
		return true;
	} else {
		return false;
	}
}

void InputHandler::handleVehicleControl(const OIS::KeyEvent& arg, bool pressed)
{
	if(mVehicle) {
		const char* t = mVehicle->getType();
		if(!strcmp(t, "f16")) {
			handlePlaneControl(arg, pressed);
		} else if(!strcmp(t, "sa8")) {
			handleLandVehicleControl(arg, pressed);
		}
	}
}

void InputHandler::handlePlaneControl(const OIS::KeyEvent& arg, bool pressed)
{
	float* val = nullptr;
	int dir = 1;
	switch(arg.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
			val = &mPitch;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			val = &mPitch;
			dir = -1;
			break;

		case OIS::KC_Q:
			val = &mYaw;
			break;

		case OIS::KC_E:
			val = &mYaw;
			dir = -1;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			val = &mRoll;
			dir = -1;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			val = &mRoll;
			break;

		default:
			break;
	}

	if(val) {
		*val = pressed ? 0.1f * dir : 0;
	}
}

void InputHandler::handleLandVehicleControl(const OIS::KeyEvent& arg, bool pressed)
{
	float* val = nullptr;
	int dir = 1;
	switch(arg.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
			val = &mAcceleration;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			val = &mAcceleration;
			dir = -1;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			val = &mYaw;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			val = &mYaw;
			dir = -1;
			break;

		default:
			break;
	}

	if(val) {
		*val = pressed ? 0.1f * dir : 0;
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
		case OIS::KC_DOWN:
		case OIS::KC_S:
		case OIS::KC_Q:
		case OIS::KC_E:
		case OIS::KC_LEFT:
		case OIS::KC_A:
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			handleVehicleControl(arg, true);
			break;

		case OIS::KC_SPACE:
			mShooting = true;
			break;

		case OIS::KC_B:
			if(mVehicle) {
				mVehicle->toggleBraking();
			}
			break;

		case OIS::KC_C:
			switch(mViewSetting) {
				case ViewSetting::Cockpit:
					mViewSetting = ViewSetting::Offset;
					break;

				case ViewSetting::Offset:
					mViewSetting = ViewSetting::Cockpit;
					break;

				default:
					break;
			}
			mViewRotation.reset();
			break;

		case OIS::KC_P:
			printInfo();
			break;

		case OIS::KC_F:
			requestVehicleChange();
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
		case OIS::KC_Q:
		case OIS::KC_E:
		case OIS::KC_LEFT:
		case OIS::KC_A:
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			handleVehicleControl(arg, false);
			break;

		default:
			break;
	}

	return true;
}

bool InputHandler::mouseMoved(const OIS::MouseEvent& arg)
{
	auto q1 = Common::Quaternion::fromAxisAngle(Common::Vector3(1, 0, 0), arg.state.Y.rel * 0.01f);
	auto q2 = Common::Quaternion::fromAxisAngle(Common::Vector3(0, 1, 0), -arg.state.X.rel * 0.01f);
	mViewRotation = q2 * mViewRotation * q1;

	return true;
}

bool InputHandler::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	if(button == OIS::MB_Left) {
		mShooting = true;
	}

	return true;
}

bool InputHandler::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}

ViewSetting InputHandler::getCurrentViewSetting() const
{
	return mViewSetting;
}

const Common::Quaternion& InputHandler::getViewRotation() const
{
	return mViewRotation;
}



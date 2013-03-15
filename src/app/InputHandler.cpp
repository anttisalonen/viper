#include "InputHandler.h"
#include "common/Math.h"

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
	mPitch = Common::clamp(-1.0f, mPitch * 1.1f, 1.0f);
	mYaw   = Common::clamp(-1.0f, mYaw   * 1.1f, 1.0f);
	mRoll  = Common::clamp(-1.0f, mRoll  * 1.1f, 1.0f);
	if(mPlane) {
		mPlane->setTargetVelocity(PrincipalAxis::Pitch, mPitch);
		mPlane->setTargetVelocity(PrincipalAxis::Yaw, mYaw);
		mPlane->setTargetVelocity(PrincipalAxis::Roll, mRoll);

		if(mShooting) {
			mShooting = false;
			mPlane->shoot();
		}
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

		case OIS::KC_SPACE:
			mShooting = true;
			break;

		case OIS::KC_V:
			{
				float roll, pitch, yaw;
				mPlane->getRotation().toEuler(roll, pitch, yaw);
				std::cout << roll << " " << pitch << " " << yaw << "\n";
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
	mViewRotation = mViewRotation * Common::Quaternion::fromAxisAngle(Common::Vector3(1, 0, 0), arg.state.X.rel * 0.01f) *
		Common::Quaternion::fromAxisAngle(Common::Vector3(0, 1, 0), arg.state.Y.rel * 0.01f);

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

ViewSetting InputHandler::getCurrentViewSetting() const
{
	return mViewSetting;
}

const Common::Quaternion& InputHandler::getViewRotation() const
{
	return mViewRotation;
}



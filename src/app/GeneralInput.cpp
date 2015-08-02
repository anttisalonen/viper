#include "common/Math.h"

#include "GeneralInput.h"
#include "General.h"

GeneralInput::GeneralInput()
{
	mCamPos = Common::Vector3(0, 100, 0);

	mCamRot = Common::Quaternion(1, 0, 0, 0);
	auto q1 = Common::Quaternion::fromAxisAngle(Common::Vector3(1, 0, 0), 0.9f * PI);
	auto q2 = Common::Quaternion::fromAxisAngle(Common::Vector3(0, 1, 0), 0.0f);
	mCamRot = q2 * mCamRot * q1;
}

void GeneralInput::setGeneral(General* g)
{
	mGeneral = g;
}

void GeneralInput::update(float time)
{
	if(time) {
		auto yvel = Common::Vector3(0.0, mCamVelocity.y, 0.0) * time * 1000.0f;
		auto vel = mCamRot.multiply(mCamVelocity) * time * 1000.0f;
		vel.y = 0.0;
		mCamPos = mCamPos + vel + yvel;
		mCamPos.y = Common::clamp(10.0f, mCamPos.y, 2500.0f);

		auto q1 = Common::Quaternion::fromAxisAngle(Common::Vector3(1, 0, 0), mRotY * time);
		auto q2 = Common::Quaternion::fromAxisAngle(Common::Vector3(0, 1, 0), mRotX * time);
		mCamRot = q2 * mCamRot * q1;

		if(mGeneral)
			mGeneral->update(time);
	}
}

bool GeneralInput::keyPressed(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
			mCamVelocity.z = 1.0f;
			break;

		case OIS::KC_DOWN:
		case OIS::KC_S:
			mCamVelocity.z = -1.0f;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
			mCamVelocity.x = 1.0f;
			break;

		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mCamVelocity.x = -1.0f;
			break;

		case OIS::KC_PGUP:
		case OIS::KC_R:
			mCamVelocity.y = 1.0f;
			break;

		case OIS::KC_PGDOWN:
		case OIS::KC_F:
			mCamVelocity.y = -1.0f;
			break;

		case OIS::KC_Q:
			mRotX = 1.0f;
			break;

		case OIS::KC_E:
			mRotX = -1.0f;
			break;

		case OIS::KC_Z:
			mRotY = 1.0f;
			break;

		case OIS::KC_X:
			mRotY = -1.0f;
			break;

		case OIS::KC_G:
			mGeneralToggle = true;
			break;

		default:
			break;
	}

	return true;
}

bool GeneralInput::keyReleased(const OIS::KeyEvent &arg)
{
	switch(arg.key) {
		case OIS::KC_UP:
		case OIS::KC_W:
		case OIS::KC_DOWN:
		case OIS::KC_S:
			mCamVelocity.z = 0.0f;
			break;

		case OIS::KC_LEFT:
		case OIS::KC_A:
		case OIS::KC_RIGHT:
		case OIS::KC_D:
			mCamVelocity.x = 0.0f;
			break;

		case OIS::KC_PGUP:
		case OIS::KC_R:
		case OIS::KC_PGDOWN:
		case OIS::KC_F:
			mCamVelocity.y = 0.0f;
			break;

		case OIS::KC_Q:
		case OIS::KC_E:
			mRotX = 0.0f;
			break;

		case OIS::KC_Z:
		case OIS::KC_X:
			mRotY = 0.0f;
			break;

		default:
			break;
	}

	return true;
}

bool GeneralInput::mouseMoved(const OIS::MouseEvent& arg)
{
	return true;
}

bool GeneralInput::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	if(button == OIS::MB_Left) {
		mMouseX = arg.state.X.abs / float(arg.state.width);
		mMouseY = arg.state.Y.abs / float(arg.state.height);
		mMouseClicked = true;
	}

	return true;
}

bool GeneralInput::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}

const Common::Vector3& GeneralInput::getCameraPosition() const
{
	return mCamPos;
}

const Common::Quaternion& GeneralInput::getCameraRotation() const
{
	return mCamRot;
}

bool GeneralInput::checkGeneralToggle()
{
	if(mGeneralToggle) {
		mGeneralToggle = false;
		return true;
	} else {
		return false;
	}
}

void GeneralInput::setCameraPosition(const Common::Vector3& pos)
{
	mCamPos = pos;
}

bool GeneralInput::mouseClicked(float& x, float& y)
{
	x = mMouseX;
	y = mMouseY;
	auto m = mMouseClicked;
	mMouseClicked = false;
	return m;
}



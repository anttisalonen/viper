#ifndef APP_GENERALINPUT_H
#define APP_GENERALINPUT_H

#include <OIS.h>

#include "common/Vector3.h"
#include "common/Quaternion.h"

class General;

class GeneralInput {
	public:
		GeneralInput();
		void setGeneral(General* g);
		void update(float time);
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);

		void setCameraPosition(const Common::Vector3& pos);
		const Common::Vector3& getCameraPosition() const;
		const Common::Quaternion& getCameraRotation() const;

		bool checkGeneralToggle();
		bool mouseClicked(float& x, float& y);

	private:
		General* mGeneral = nullptr;
		Common::Quaternion mCamRot;
		Common::Vector3 mCamPos;
		Common::Vector3 mCamVelocity;

		float mRotX = 0.0f;
		float mRotY = 0.0f;

		bool mGeneralToggle = false;
		bool mMouseClicked = false;
		float mMouseX = 0.0f;
		float mMouseY = 0.0f;
};


#endif


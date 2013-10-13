#ifndef APP_INPUTHANDLER_H
#define APP_INPUTHANDLER_H

#include <OIS.h>

#include "common/Quaternion.h"

#include "VehicleController.h"

enum class ViewSetting {
	Offset,
	Cockpit
};

class InputHandler : public VehicleController, public OIS::KeyListener, public OIS::MouseListener {
	public:
		InputHandler();
		virtual void update(float time) override;
		bool running() const;
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		ViewSetting getCurrentViewSetting() const;
		const Common::Quaternion& getViewRotation() const;
		bool checkVehicleChangeRequest();
		bool checkGeneralToggle();
		int getSide() const override;

	private:
		void printInfo();
		void requestVehicleChange();
		void handleVehicleControl(const OIS::KeyEvent& arg, bool pressed);
		void handlePlaneControl(const OIS::KeyEvent& arg, bool pressed);
		void handleLandVehicleControl(const OIS::KeyEvent& arg, bool pressed);

		float mPitch = 0.0f;
		float mRoll = 0.0f;
		float mYaw = 0.0f;
		float mAcceleration = 0.0f;
		bool mRunning = true;
		bool mShooting = false;
		ViewSetting mViewSetting = ViewSetting::Cockpit;
		Common::Quaternion mViewRotation;

		bool mRequestingVehicleChange = false;
		bool mToggleGeneral = false;
};

#endif


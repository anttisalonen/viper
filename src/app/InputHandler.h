#ifndef APP_INPUTHANDLER_H
#define APP_INPUTHANDLER_H

#include <OIS.h>

#include "PlaneController.h"

enum class ViewSetting {
	Offset,
	Cockpit
};

class InputHandler : public PlaneController, public OIS::KeyListener, public OIS::MouseListener {
	public:
		InputHandler();
		virtual void update(float time) override;
		bool running() const;
		virtual void planeReset() override;
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		ViewSetting getCurrentViewSetting() const;

	private:
		float mPitch = 0.0f;
		float mRoll = 0.0f;
		float mYaw = 0.0f;
		bool mRunning = true;
		bool mShooting = false;
		ViewSetting mViewSetting = ViewSetting::Cockpit;
};

#endif


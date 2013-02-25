#ifndef APP_INPUTHANDLER_H
#define APP_INPUTHANDLER_H

#include <OIS.h>

class InputHandler : public OIS::KeyListener, public OIS::MouseListener {
	public:
		InputHandler();
		bool update(float time);
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);

	private:
		float mForward = 0.0;
		bool mRunning = true;
};

#endif


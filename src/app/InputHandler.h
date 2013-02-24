#ifndef APP_INPUTHANDLER_H
#define APP_INPUTHANDLER_H

#include <OIS.h>

class App;

class InputHandler : public OIS::KeyListener, public OIS::MouseListener {
	public:
		InputHandler(App* app);
		bool frameRendered(float time);
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);

	private:
		App* mApp;
		float mForward = 0.0;
		bool mRunning = true;
};

#endif


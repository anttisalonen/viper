#ifndef APP_USERINPUT_H
#define APP_USERINPUT_H

#include <OIS.h>

class UserInput {
	public:
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool running() const;

	private:
		bool mRunning = true;
};


#endif


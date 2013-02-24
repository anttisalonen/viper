#ifndef APP_APP_H
#define APP_APP_H

#include <Ogre.h>
#include <OIS.h>

#include "common/Clock.h"

class InputHandler;

class App {
	public:
		App();
		~App();
		void go();
		Ogre::Camera* getCamera();
		Ogre::SceneNode* getPlaneNode();

	private:
		void initResources();
		void initInput();
		bool checkWindowResize();
		void setupScene();

		Ogre::Root* mRoot = nullptr;
		Ogre::RenderWindow* mWindow = nullptr;
		Ogre::SceneManager* mSceneMgr = nullptr;
		Ogre::SceneNode* mRootNode = nullptr;
		Ogre::Camera* mCamera = nullptr;
		Ogre::SceneNode* mCamNode = nullptr;
		Ogre::Viewport* mViewport = nullptr;
		Ogre::RaySceneQuery* mRaySceneQuery = nullptr;

		InputHandler* mInputHandler = nullptr;
		OIS::InputManager* mInputManager = nullptr;
		OIS::Keyboard* mKeyboard = nullptr;
		OIS::Mouse* mMouse = nullptr;

		unsigned int mWindowWidth = 0;
		unsigned int mWindowHeight = 0;

		Ogre::Entity*         mOceanSurfaceEnt = nullptr;
		Ogre::Entity*         mPlaneEnt = nullptr;
		Ogre::SceneNode* mPlaneNode = nullptr;

		Common::Clock mFPSTimer;
};

#endif


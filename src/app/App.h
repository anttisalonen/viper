#ifndef APP_APP_H
#define APP_APP_H

#include <map>

#include <Ogre.h>
#include <OIS.h>

#include "common/Clock.h"
#include "common/Vector3.h"
#include "common/Quaternion.h"

class InputHandler;
class Game;
class VisibleEntity;

class App {
	public:
		App();
		~App();
		void go();
		void updatePlane(const VisibleEntity* p,
				const Common::Vector3& v,
				const Common::Quaternion& q);
		void setCamera(const Common::Vector3& offset,
				const Common::Vector3& lookat,
				const Common::Quaternion& rot);
		void setCamera(const Common::Vector3& position,
				const Common::Quaternion& rot);

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

		Game* mGame = nullptr;
		InputHandler* mInputHandler = nullptr;
		OIS::InputManager* mInputManager = nullptr;
		OIS::Keyboard* mKeyboard = nullptr;
		OIS::Mouse* mMouse = nullptr;

		unsigned int mWindowWidth = 0;
		unsigned int mWindowHeight = 0;

		Ogre::Entity*         mOceanSurfaceEnt = nullptr;

		Common::Clock mFPSTimer;

		std::map<const VisibleEntity*, Ogre::Entity*> mEntities;

		unsigned int mNumEntities = 0;
};

#endif


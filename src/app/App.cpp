#include "App.h"

#include "InputHandler.h"

#define APP_RESOURCE_NAME "Resources"

App::App()
{
	mRoot = new Ogre::Root("", "", "");
	mRoot->loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_GL");
	mRoot->loadPlugin(OGRE_PLUGIN_DIR "/Plugin_OctreeSceneManager");

	const Ogre::RenderSystemList& rsys = mRoot->getAvailableRenderers();
	if(rsys.size() == 0) {
		delete mRoot;
		throw std::runtime_error("No rendersystems found.\n");
	}
	else {
		mRoot->setRenderSystem(rsys[0]);
		mRoot->initialise(false, "", "");
		Ogre::NameValuePairList params;
		params["FSAA"] = "4";
		params["vsync"] = "false";
		params["border"] = "fixed";
		mWindowWidth = 1280;
		mWindowHeight = 720;
		mWindow = mRoot->createRenderWindow("Render Window", mWindowWidth, mWindowHeight, false, &params);
		if(!mWindow) {
			delete mRoot;
			throw std::runtime_error("Could not create the render window.\n");
		}
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
		mRootNode = mSceneMgr->getRootSceneNode();
		mCamera = mSceneMgr->createCamera("Camera");
		mCamNode = mRootNode->createChildSceneNode("CameraNode");
		mCamNode->attachObject(mCamera);
		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		mCamera->setAspectRatio(float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));
		mCamera->setNearClipDistance(1.5f);
		mCamera->setFarClipDistance(3000.0f);

		mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

		initResources();

		mInputHandler = new InputHandler(this);
		initInput();

		setupScene();
		checkWindowResize();
	}
}

void App::initResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/ocean", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/aircraft", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(APP_RESOURCE_NAME);
}

void App::initInput()
{
	size_t hWnd = 0;
	mWindow->getCustomAttribute("WINDOW", &hWnd);
	OIS::ParamList pl;
	std::ostringstream windowHndStr;

	windowHndStr << hWnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if defined OIS_WIN32_PLATFORM
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
	pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	mInputManager = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(mInputHandler);
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(mInputHandler);
}

bool App::checkWindowResize()
{
	unsigned int mOldWidth, mOldHeight;
	mOldWidth = mWindowWidth;
	mOldHeight = mWindowHeight;
	mWindowWidth = mWindow->getWidth();
	mWindowHeight = mWindow->getHeight();
	if(mWindowWidth != mOldWidth || mWindowHeight != mOldHeight) {
		mMouse->getMouseState().width = mWindowWidth;
		mMouse->getMouseState().height = mWindowHeight;
		mCamera->setAspectRatio(float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));
		return true;
	}
	return false;
}

void App::setupScene()
{
	// based on OceanDemo
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	mSceneMgr->setSkyBox(true, "SkyBox", 1000);

	mCamera->moveRelative(Ogre::Vector3(50, 20, 100));
	mCamera->lookAt(0, 0, 0);

	// Define a plane mesh that will be used for the ocean surface
	Ogre::Plane oceanSurface;
	oceanSurface.normal = Ogre::Vector3::UNIT_Y;
	oceanSurface.d = 0;
	Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
			APP_RESOURCE_NAME,
			oceanSurface,
			1000, 1000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	mOceanSurfaceEnt = mSceneMgr->createEntity("OceanSurface", "OceanSurface");
	mOceanSurfaceEnt->setMaterialName("OceanSurface");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mOceanSurfaceEnt);

	mPlaneEnt = mSceneMgr->createEntity("Plane", "f16.mesh");
	mPlaneEnt->setMaterialName("Material");
	mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90.0f));
	mPlaneNode->setPosition(20, 25, 50);
	mPlaneNode->attachObject(mPlaneEnt);
}

App::~App()
{
	delete mInputHandler;
	delete mRoot;
}

void App::go()
{
	double prevTime = Common::Clock::getTime();
	bool running = true;
	while(running && !mWindow->isClosed()) {
		mRoot->renderOneFrame();
		checkWindowResize();
		Ogre::WindowEventUtilities::messagePump();
		mKeyboard->capture();
		mMouse->capture();
		double thisTime = Common::Clock::getTime();
		double diffTime = thisTime - prevTime;
		prevTime = thisTime;
		if(!mInputHandler->frameRendered(diffTime)) {
			running = false;
		}
		mFPSTimer.limitFPS(60);
	}
}

Ogre::Camera* App::getCamera()
{
	return mCamera;
}

Ogre::SceneNode* App::getPlaneNode()
{
	return mPlaneNode;
}



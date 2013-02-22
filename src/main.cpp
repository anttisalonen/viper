#include <iostream>
#include <stdexcept>

#include <Ogre.h>
#include <OIS.h>

#define APP_RESOURCE_NAME "Resources"

class App : public OIS::KeyListener, public OIS::MouseListener {
	public:
		App();
		~App();
		void go();
		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);
		bool mouseMoved(const OIS::MouseEvent& arg);
		bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button);
		bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button);

	private:
		void initResources();
		void initInput();
		bool checkWindowResize();

		Ogre::Root* mRoot = nullptr;
		Ogre::RenderWindow* mWindow = nullptr;
		Ogre::SceneManager* mScene = nullptr;
		Ogre::SceneNode* mRootNode = nullptr;
		Ogre::Camera* mCamera = nullptr;
		Ogre::SceneNode* mCamNode = nullptr;
		Ogre::Viewport* mViewport = nullptr;
		Ogre::RaySceneQuery* mRaySceneQuery = nullptr;

		OIS::InputManager* mInputManager = nullptr;
		OIS::Keyboard* mKeyboard = nullptr;
		OIS::Mouse* mMouse = nullptr;

		bool mRunning = false;
		unsigned int mWindowWidth = 0;
		unsigned int mWindowHeight = 0;
};

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
		params["FSAA"] = "0";
		params["vsync"] = "false";
		params["border"] = "fixed";
		mWindowWidth = 1280;
		mWindowHeight = 720;
		mWindow = mRoot->createRenderWindow("Render Window", mWindowWidth, mWindowHeight, false, &params);
		if(!mWindow) {
			delete mRoot;
			throw std::runtime_error("Could not create the render window.\n");
		}
		mScene = mRoot->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
		mRootNode = mScene->getRootSceneNode();
		mCamera = mScene->createCamera("Camera");
		mCamNode = mRootNode->createChildSceneNode("CameraNode");
		mCamNode->attachObject(mCamera);
		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		mCamera->setAspectRatio(float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));
		mCamera->setNearClipDistance(1.5f);
		mCamera->setFarClipDistance(3000.0f);
		mCamNode->setPosition(64, 64, 50.0f);
		mCamera->lookAt(64, 64, 0);

		mRaySceneQuery = mScene->createRayQuery(Ogre::Ray());

		initResources();

		initInput();
		checkWindowResize();

		mRunning = true;
	}
}

void App::initResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share", "FileSystem", APP_RESOURCE_NAME, false);
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
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
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
		std::cout << "New window size: " << mWindowWidth << " x " << mWindowHeight << "\n";
		return true;
	}
	return false;
}

App::~App()
{
	delete mRoot;
}

void App::go()
{
}

bool App::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool App::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool App::mouseMoved(const OIS::MouseEvent& arg)
{
	return true;
}

bool App::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}

bool App::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID button)
{
	return true;
}


int main(int argc, char** argv)
{
	try {
		App app;
		app.go();
	} catch (Ogre::Exception& e) {
		std::cerr << "Ogre exception: " << e.what() << std::endl;
		return 1;
	} catch (std::exception& e) {
		std::cerr << "std::exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

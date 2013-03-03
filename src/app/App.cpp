#include "App.h"

#include "InputHandler.h"
#include "Game.h"
#include "Entity.h"
#include "Missile.h"

#define APP_RESOURCE_NAME "Resources"

App::App()
{
	mRoot = new Ogre::Root("", "", "");
	mRoot->loadPlugin(OGRE_PLUGIN_DIR "/RenderSystem_GL");
	mRoot->loadPlugin(OGRE_PLUGIN_DIR "/Plugin_OctreeSceneManager");
	mRoot->loadPlugin(OGRE_PLUGIN_DIR "/Plugin_ParticleFX");

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
		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		mCamera->setAspectRatio(float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));
		mCamera->setNearClipDistance(5.0f);
		mCamera->setFarClipDistance(3000.0f);
		mCamera->lookAt(20, 25, 50);

		mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

		initResources();

		mInputHandler = new InputHandler();
		initInput();

		setupScene();
		checkWindowResize();

		mGame = new Game(this, mInputHandler);
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
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	mSceneMgr->setSkyBox(true, "SkyBox", 1000);

	Ogre::Light* sun = mSceneMgr->createLight("sun");
	sun->setType(Ogre::Light::LT_DIRECTIONAL);
	sun->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
	sun->setSpecularColour(Ogre::ColourValue(1, 1, 1));
	sun->setDirection(0, -1, 0);


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

	mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
	mCamNode->attachObject(mCamera);
	mCamera->moveRelative(Ogre::Vector3(0, 5, -20));
}

App::~App()
{
	delete mGame;
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
		if(!mGame->update(diffTime)) {
			running = false;
		}
		mFPSTimer.limitFPS(60, false);
	}
}

void App::updateEntity(const VisibleEntity* p)
{
	const Common::Vector3& v = p->getPosition();
	const Common::Quaternion& q = p->getRotation();
	Ogre::Entity* e;
	Ogre::SceneNode* n;
	auto it = mEntities.find(p);

	if(it == mEntities.end()) {
		char entityname[256];
		char meshname[256];
		snprintf(entityname, 255, "Entity %4d", ++mNumEntities);
		snprintf(meshname, 255, "%s.mesh", p->getType());
		e = mSceneMgr->createEntity(entityname, meshname);

		n = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		n->attachObject(e);

		mEntities.insert({p, e});
	} else {
		e = it->second;
		n = e->getParentSceneNode();
	}

	n->setOrientation(q.w, q.x, q.y, q.z);
	n->setPosition(v.x, v.y, v.z);
}

void App::removeEntity(const VisibleEntity* p)
{
	auto it = mEntities.find(p);
	if(it != mEntities.end()) {
		Ogre::Entity* e = it->second;
		auto n = e->getParentSceneNode();
		e->detachFromParent();
		mSceneMgr->destroyEntity(e);
		mSceneMgr->destroySceneNode(n);
		mEntities.erase(it);
	}
}

void App::updateMissile(const Missile* m)
{
	updateEntity(m);
	if(!m->attached()) {
		Ogre::SceneNode* n;
		auto eit = mEntities.find(m);
		assert(eit != mEntities.end());
		n = eit->second->getParentSceneNode();

		Ogre::ParticleSystem* ps;
		auto it = mMissileEngines.find(m);

		if(it == mMissileEngines.end()) {
			char instancename[256];
			snprintf(instancename, 255, "MissileEngine %4d", ++mNumMissileEngines);
			ps = mSceneMgr->createParticleSystem(instancename, "MissileEngine");
			n->attachObject(ps);

			mMissileEngines.insert({m, ps});
		} else {
			ps = it->second;
		}
	}
}

void App::removeMissile(const Missile* m)
{
	auto it = mMissileEngines.find(m);
	if(it != mMissileEngines.end()) {
		Ogre::ParticleSystem* ps = it->second;
		ps->detachFromParent();
		mSceneMgr->destroyParticleSystem(ps);
		mMissileEngines.erase(it);
	}

	removeEntity(m);
}

void App::setCamera(const Common::Vector3& offset,
		const Common::Vector3& lookat,
		const Common::Quaternion& rot)
{
	// TODO: make camera transitions smooth (both position and orientation)
	Ogre::Quaternion q(rot.w, rot.x, rot.y, rot.z);
	Ogre::Vector3 l(lookat.x, lookat.y, lookat.z);
	Ogre::Vector3 o(offset.x, offset.y, offset.z);
	mCamera->setPosition(l + (q * o));
	mCamera->lookAt(l);
}

void App::setCamera(const Common::Vector3& position,
		const Common::Quaternion& rot)
{
	mCamera->setPosition(Ogre::Vector3(position.x, position.y, position.z));
	// NOTE: the x and z axes are inverted, probably because of the model
	// geometry
	mCamera->setOrientation(Ogre::Quaternion(rot.w, -rot.x, rot.y, -rot.z));
	mCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180));
}


#include <Overlay/OgreOverlaySystem.h>
#include "UserInterface.h"

#include "TextRenderer.h"
#include "MouseCursor.h"
#include "InputHandler.h"
#include "GeneralInput.h"
#include "Game.h"
#include "Entity.h"
#include "Missile.h"
#include "Vehicle.h"
#include "Terrain.h"

#include "common/Math.h"

#define APP_RESOURCE_NAME "Resources"

UserInterface::UserInterface(InputHandler* ih, const Terrain* t)
	: mConstants("share/graphics/config/constants.json"),
	mTerrain(t)
{
        // get user data directory
        char* homedir = getenv("HOME");
        if(homedir) {
		std::string userdatadir = std::string(homedir) + "/.viper/";
                struct stat st;
                if(stat(userdatadir.c_str(), &st) != 0) {
                        mkdir(userdatadir.c_str(), 0755);
                }
                // suppress ogre output to stderr
                Ogre::LogManager* logMgr = new Ogre::LogManager();
		logMgr->createLog(userdatadir + "Ogre.log", true, false, false);
	}

	mRoot = new Ogre::Root("", "", "ogre.log");
	const char* ogrePluginDir = getenv("OGRE_PLUGIN_DIR");
	if(!ogrePluginDir)
		ogrePluginDir = OGRE_PLUGIN_DIR;
	if(!ogrePluginDir)
		ogrePluginDir = ".";
	std::string ogrePluginDirStr = std::string(ogrePluginDir);
	mRoot->loadPlugin(ogrePluginDirStr + "/RenderSystem_GL");
	mRoot->loadPlugin(ogrePluginDirStr + "/Plugin_OctreeSceneManager");
	mRoot->loadPlugin(ogrePluginDirStr + "/Plugin_ParticleFX");

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
		params["vsync"] = "true";
		params["border"] = "fixed";
		mWindowWidth = 1280;
		mWindowHeight = 720;
		mWindow = mRoot->createRenderWindow("Render Window", mWindowWidth, mWindowHeight, false, &params);
		if(!mWindow) {
			delete mRoot;
			throw std::runtime_error("Could not create the render window.\n");
		}
		//mWindow->setFullscreen(true, 1360, 768);
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_FAR, "SceneManager");
		Ogre::OverlaySystem* pOverlaySystem = new Ogre::OverlaySystem();
		mSceneMgr->addRenderQueueListener(pOverlaySystem);
		if(mConstants.getBool("shadows")) {
			mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
		}
		mRootNode = mSceneMgr->getRootSceneNode();
		mCamera = mSceneMgr->createCamera("Camera");
		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		mCamera->setAspectRatio(float(mViewport->getActualWidth()) / float(mViewport->getActualHeight()));
		mCamera->setNearClipDistance(1.0f);
		mCamera->setFarClipDistance(3000.0f);
		mCamera->lookAt(20, 25, 50);

		mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

		setupDebug();

		initResources();

		initInput(ih);

		setupScene();
		checkWindowResize();

		new TextRenderer();
		TextRenderer::getSingleton().addTextBox("position", "Position: ", 10, 10, 100, 20, Ogre::ColourValue::White);

		mMouseCursor = new MouseCursor();
		mMouseCursor->setImage("cursor.png");
		mMouseCursor->setVisible(false);
		mMouseCursor->setWindowDimensions(mWindowWidth, mWindowHeight);
	}
}

void UserInterface::initResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/aircraft", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/ocean", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/terrain", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/vehicles", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/ui", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(APP_RESOURCE_NAME);
}

void UserInterface::initInput(InputHandler* ih)
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
	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("true")));
	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
	pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	mInputManager = OIS::InputManager::createInputSystem(pl);
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(ih);
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(ih);
}

bool UserInterface::checkWindowResize()
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
		mMouseCursor->setWindowDimensions(mWindowWidth, mWindowHeight);
		return true;
	}
	return false;
}

void UserInterface::setupScene()
{
	// Set ambient light
	auto sunColour = mConstants.getVector3("sun_light_colour");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(sunColour.x * 0.5f, sunColour.y * 0.5f, sunColour.z * 0.5f));
	mSceneMgr->setSkyBox(true, "SkyBox", 1000);

	Ogre::Light* sun = mSceneMgr->createLight("sun");
	sun->setType(Ogre::Light::LT_DIRECTIONAL);
	sun->setDiffuseColour(Ogre::ColourValue(sunColour.x, sunColour.y, sunColour.z));
	sun->setSpecularColour(Ogre::ColourValue(sunColour.x, sunColour.y, sunColour.z));
	auto sunLightDirection = mConstants.getVector3("directional_light");
	sun->setDirection(sunLightDirection.x, sunLightDirection.y, sunLightDirection.z);


	// Define a plane mesh that will be used for the ocean surface
	Ogre::Plane oceanSurface;
	oceanSurface.normal = Ogre::Vector3::UNIT_Y;
	oceanSurface.d = 0;
	Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
			APP_RESOURCE_NAME,
			oceanSurface,
			10000, 10000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	mOceanSurfaceEnt = mSceneMgr->createEntity("OceanSurface", "OceanSurface");
	mOceanSurfaceEnt->setMaterialName("OceanSurface");
	mOceanSurfaceEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mOceanSurfaceEnt);

	mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
	mCamNode->attachObject(mCamera);
	mCamera->moveRelative(Ogre::Vector3(0, 5, -20));

	setupTerrain();
}

void UserInterface::setupTerrain()
{
	// Plates define the number of manual objects fed to Ogre.
	// numQuads^2 quads make up one plate.
	int terrainNum = 1;
	const float scale = 10.0f;
	const int numQuads = mConstants.getUInt("terrain_quads_per_plate");
	const int numPlates = mTerrain->getDimension() / (numQuads * scale);
	const float offset = scale * numQuads;
	const float terrainTextureScale = mConstants.getFloat("terrain_texture_scale");

	for(int x = 0; x < numPlates; x++) {
		for(int y = 0; y < numPlates; y++) {
			char terrainName[32];
			sprintf(terrainName, "Terrain %d", terrainNum);
			Ogre::ManualObject* obj = mSceneMgr->createManualObject(terrainName);
			obj->setDynamic(false);
			obj->setCastShadows(false);
			obj->begin("Terrain", Ogre::RenderOperation::OT_TRIANGLE_LIST);

			unsigned int ii = 0;
			const float offsetx = offset * (x - numPlates / 2);
			const float offsety = offset * (y - numPlates / 2);

			for(unsigned int i = 0; i < numQuads; i++) {
				for(unsigned int j = 0; j < numQuads; j++) {
					/*		x1 - 1	x1	x2	x1 + 1
					 * y1 - 1		U1	U2
					 * y1		L1	z1	z4	R1
					 * y2		L2	z2	z3	R2
					 * y2 + 1		D1	D2
					 *
					 * finite difference method */
					float x1 = offsetx + scale * i;
					float x2 = offsetx + scale * (i + 1);
					float y1 = offsety + scale * j;
					float y2 = offsety + scale * (j + 1);
					float z1 = getTerrainHeightAt(x1, y1);
					float z2 = getTerrainHeightAt(x1, y2);
					float z3 = getTerrainHeightAt(x2, y2);
					float z4 = getTerrainHeightAt(x2, y1);
					float zL1 = getTerrainHeightAt(x1 - scale, y1);
					float zL2 = getTerrainHeightAt(x1 - scale, y2);
					float zR1 = getTerrainHeightAt(x2 + scale, y1);
					float zR2 = getTerrainHeightAt(x2 + scale, y2);
					float zU1 = getTerrainHeightAt(x1, y1 - scale);
					float zU2 = getTerrainHeightAt(x2, y1 - scale);
					float zD1 = getTerrainHeightAt(x1, y2 + scale);
					float zD2 = getTerrainHeightAt(x2, y2 + scale);
					Ogre::Vector3 n1(zL1 - z4, 2.0, zU1 - z2); n1.normalise();
					Ogre::Vector3 n2(zL2 - z3, 2.0, z1 - zD1); n2.normalise();
					Ogre::Vector3 n3(z2 - zR2, 2.0, z4 - zD2); n3.normalise();
					Ogre::Vector3 n4(z1 - zR1, 2.0, zU2 - z3); n4.normalise();
					float fx1 = fmod(x1 * terrainTextureScale, 1.0f);
					float fx2 = fmod(x2 * terrainTextureScale, 1.0f);
					float fy1 = fmod(y1 * terrainTextureScale, 1.0f);
					float fy2 = fmod(y2 * terrainTextureScale, 1.0f);
					obj->position(x1, z1, y1); obj->normal(n1); obj->textureCoord(fx1, fy1);
					obj->position(x1, z2, y2); obj->normal(n2); obj->textureCoord(fx1, fy2);
					obj->position(x2, z3, y2); obj->normal(n3); obj->textureCoord(fx2, fy2);
					obj->position(x2, z4, y1); obj->normal(n4); obj->textureCoord(fx2, fy1);
					obj->quad(ii, ii + 1, ii + 2, ii + 3);
					ii += 4;
				}
			}

			obj->end();
			mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(obj);

			terrainNum++;
		}
	}
}

float UserInterface::getTerrainHeightAt(float x, float y) const
{
	return mTerrain->getHeightAt(x, y);
}

UserInterface::~UserInterface()
{
	delete mRoot;
}

bool UserInterface::isClosed() const
{
	return !mWindow || mWindow->isClosed();
}

void UserInterface::renderOneFrame()
{
	if(!isClosed()) {
		mRoot->renderOneFrame();
		checkWindowResize();
		Ogre::WindowEventUtilities::messagePump();
		mKeyboard->capture();
		mMouse->capture();
		mMouseCursor->updatePosition(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs);
	}
}

Ogre::Entity* UserInterface::ogreEntityFromVisibleEntity(const VisibleEntity* v)
{
	Ogre::Entity* e = nullptr;
	Ogre::SceneNode* n;
	auto it = mEntities.find(v);

	if(it == mEntities.end()) {
		char entityname[256];
		char meshname[256];
		snprintf(entityname, 255, "Entity %4d", ++mNumEntities);
		snprintf(meshname, 255, "%s.mesh", v->getType());
		e = mSceneMgr->createEntity(entityname, meshname);
		e->setCastShadows(true);

		n = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		n->attachObject(e);

		mEntities.insert({v, e});
	} else {
		e = it->second;
		n = e->getParentSceneNode();
	}

	assert(e);
	return e;
}

void UserInterface::updateEntity(const VisibleEntity* p)
{
	const Common::Vector3& v = p->getPosition();
	const Common::Quaternion& q = p->getRotation();
	Ogre::Entity* e = ogreEntityFromVisibleEntity(p);
	Ogre::SceneNode* n = e->getParentSceneNode();
	n->setOrientation(q.w, q.x, q.y, q.z);
	n->setPosition(v.x, v.y, v.z);
}

void UserInterface::removeEntity(const VisibleEntity* p)
{
	{
		auto it = mParticleSystems.find(p);
		if(it != mParticleSystems.end()) {
			Ogre::ParticleSystem* ps = it->second;
			ps->detachFromParent();
			mSceneMgr->destroyParticleSystem(ps);
			mParticleSystems.erase(it);
		}
	}

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
}

void UserInterface::updateVehicle(const Vehicle* p)
{
	updateEntity(p);

	if(p->isDestroyed()) {
		checkAddParticleSystem(p, "DestroyedVehicle");
	}
}

void UserInterface::checkAddParticleSystem(const VisibleEntity* m, const char* type)
{
	Ogre::SceneNode* n;
	auto eit = mEntities.find(m);
	assert(eit != mEntities.end());
	n = eit->second->getParentSceneNode();

	Ogre::ParticleSystem* ps;
	auto it = mParticleSystems.find(m);

	if(it == mParticleSystems.end()) {
		char instancename[256];
		snprintf(instancename, 255, "ParticleSystem %4d", ++mNumParticleSystems);
		ps = mSceneMgr->createParticleSystem(instancename, type);
		n->attachObject(ps);

		mParticleSystems.insert({m, ps});
	} else {
		ps = it->second;
	}
}

void UserInterface::updateMissile(const Missile* m)
{
	updateEntity(m);
	if(!m->attached()) {
		checkAddParticleSystem(m, "MissileEngine");
	}
}

void UserInterface::removeMissile(const Missile* m)
{
	removeEntity(m);
}

void UserInterface::setCamera(const Common::Vector3& position,
		const Common::Vector3& lookat)
{
	mCamera->setPosition(Ogre::Vector3(position.x, position.y, position.z));
	mCamera->lookAt(Ogre::Vector3(lookat.x, lookat.y, lookat.z));
	constrainCamera();
	updateTextLabels();
}

void UserInterface::setCamera(const Common::Vector3& offset,
		const Common::Vector3& lookat,
		const Common::Quaternion& rot)
{
	// TODO: make camera transitions smooth (both position and orientation)
	Ogre::Quaternion q(rot.w, rot.x, rot.y, rot.z);
	Ogre::Vector3 l(lookat.x, lookat.y, lookat.z);
	Ogre::Vector3 o(offset.x, offset.y, offset.z);
	mCamera->setPosition(l + (q * o));
	mCamera->lookAt(l);
	constrainCamera();
	updateTextLabels();
}

void UserInterface::setCamera(const Common::Vector3& position,
		const Common::Quaternion& rot)
{
	mCamera->setPosition(Ogre::Vector3(position.x, position.y, position.z));
	// NOTE: the x and z axes are inverted, probably because of the model
	// geometry
	mCamera->setOrientation(Ogre::Quaternion(rot.w, -rot.x, rot.y, -rot.z));
	mCamera->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180));
	constrainCamera();
	updateTextLabels();
}

void UserInterface::updateTextLabels()
{
	const Ogre::Vector3& pos = mCamera->getPosition();
	TextRenderer::getSingleton().printf("position", "Position: %3.2f %3.2f %3.2f",
			pos.x, pos.y, pos.z);
}

void UserInterface::constrainCamera()
{
	const Ogre::Vector3& pos = mCamera->getPosition();
	float minHeight = std::max<float>(0.0f, getTerrainHeightAt(pos.x, pos.z)) + 2.0f;
	if(minHeight > pos.y) {
		mCamera->setPosition(pos.x, minHeight, pos.z);
	}
}

void UserInterface::setupDebug()
{
	Ogre::MaterialPtr debugMaterial = Ogre::MaterialManager::getSingleton().create("debugMaterial", "General"); 
	debugMaterial->setReceiveShadows(false); 
	debugMaterial->getTechnique(0)->setLightingEnabled(true); 
	debugMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	debugMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	debugMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 

}

void UserInterface::setMouseVisible(bool b)
{
	mMouseCursor->setVisible(b);
}

void UserInterface::raycast(float x, float y, Common::Vector3& origin, Common::Vector3& dir) const
{
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(x, y);
	auto ori = mouseRay.getOrigin();
	auto di = mouseRay.getDirection();
	origin.x = ori.x;
	origin.y = ori.y;
	origin.z = ori.z;
	dir.x = di.x;
	dir.y = di.y;
	dir.z = di.z;
}

void UserInterface::setEntityHighlight(const VisibleEntity* e, bool h)
{
	auto oe = ogreEntityFromVisibleEntity(e);
	Ogre::SceneNode* n = oe->getParentSceneNode();
	n->showBoundingBox(h);
}



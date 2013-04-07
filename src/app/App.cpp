#include <noise/noise.h>

#include "App.h"

#include "InputHandler.h"
#include "Game.h"
#include "Entity.h"
#include "Missile.h"
#include "Plane.h"

#define APP_RESOURCE_NAME "Resources"

App::App(InputHandler* ih)
	: mConstants("share/graphics/config/constants.json")
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
		if(mConstants.getBool("shadows"))
			mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
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

		initInput(ih);

		setupScene();
		checkWindowResize();
	}
}

void App::initResources()
{
	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/aircraft", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/ocean", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("share/graphics/terrain", "FileSystem", APP_RESOURCE_NAME, false);
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(APP_RESOURCE_NAME);
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(APP_RESOURCE_NAME);
}

void App::initInput(InputHandler* ih)
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

void App::setupTerrain()
{
	int terrainNum = 1;
	const float scale = 10.0f;
	const unsigned int numTiles = mConstants.getUInt("terrain_tiles");
	const unsigned int numPlates = 16;
	const float offset = scale * numTiles;
	const float terrainTextureScale = mConstants.getFloat("terrain_texture_scale");

	for(int x = 0; x < numPlates; x++) {
		for(int y = 0; y < numPlates; y++) {
			char terrainName[32];
			sprintf(terrainName, "Terrain %d", terrainNum);
			Ogre::ManualObject* obj = mSceneMgr->createManualObject(terrainName);
			obj->setDynamic(false);
			obj->begin("Terrain", Ogre::RenderOperation::OT_TRIANGLE_LIST);

			unsigned int ii = 0;
			const float offsetx = offset * (x - numPlates / 2);
			const float offsety = offset * (y - numPlates / 2);

			for(unsigned int i = 0; i < numTiles; i++) {
				for(unsigned int j = 0; j < numTiles; j++) {
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

float App::getTerrainHeightAt(float x, float y) const
{
	static noise::module::Perlin perlin;
	static float scale = mConstants.getFloat("terrain_height_scale");
	static float offset = mConstants.getFloat("terrain_height_offset");
	return offset + scale * perlin.GetValue(x * 0.001f, y * 0.001f, 0.0f);
}

App::~App()
{
	delete mRoot;
}

bool App::isClosed() const
{
	return !mWindow || mWindow->isClosed();
}

void App::renderOneFrame()
{
	if(!isClosed()) {
		mRoot->renderOneFrame();
		checkWindowResize();
		Ogre::WindowEventUtilities::messagePump();
		mKeyboard->capture();
		mMouse->capture();
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

void App::updatePlane(const Plane* p)
{
	updateEntity(p);

	if(p->isDestroyed()) {
		checkAddParticleSystem(p, "DestroyedPlane");
	}
}

void App::checkAddParticleSystem(const VisibleEntity* m, const char* type)
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

void App::updateMissile(const Missile* m)
{
	updateEntity(m);
	if(!m->attached()) {
		checkAddParticleSystem(m, "MissileEngine");
	}
}

void App::removeMissile(const Missile* m)
{
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


#ifndef APP_APP_H
#define APP_APP_H

#include <map>

#include <Ogre.h>
#include <OIS.h>

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Constants.h"

class InputHandler;
class Game;
class VisibleEntity;
class Missile;
class Vehicle;
class Terrain;
class MouseCursor;

class UserInterface {
	public:
		UserInterface(InputHandler* ih, const Terrain* t);
		~UserInterface();
		bool isClosed() const;
		void renderOneFrame();
		void updateEntity(const VisibleEntity* p);
		void removeEntity(const VisibleEntity* p);
		void updateVehicle(const Vehicle* p);
		void updateMissile(const Missile* m);
		void removeMissile(const Missile* m);
		void setCamera(const Common::Vector3& position,
				const Common::Vector3& lookat);
		void setCamera(const Common::Vector3& offset,
				const Common::Vector3& lookat,
				const Common::Quaternion& rot);
		void setCamera(const Common::Vector3& position,
				const Common::Quaternion& rot);
		void setMouseVisible(bool b);
		void raycast(float x, float y, Common::Vector3& origin, Common::Vector3& dir) const;
		void setEntityHighlight(const VisibleEntity* e, bool h);

	private:
		void initResources();
		void initInput(InputHandler* ih);
		bool checkWindowResize();
		void setupScene();
		void setupTerrain();
		void addTerrainPosition(Ogre::ManualObject* obj, float x, float y);
		float getTerrainHeightAt(float x, float y) const;
		void checkAddParticleSystem(const VisibleEntity* m, const char* type);
		void constrainCamera();
		void setupDebug();
		void updateTextLabels();

		Ogre::Entity* ogreEntityFromVisibleEntity(const VisibleEntity* v);

		Ogre::Root* mRoot = nullptr;
		Ogre::RenderWindow* mWindow = nullptr;
		Ogre::SceneManager* mSceneMgr = nullptr;
		Ogre::SceneNode* mRootNode = nullptr;
		Ogre::Camera* mCamera = nullptr;
		Ogre::SceneNode* mCamNode = nullptr;
		Ogre::Viewport* mViewport = nullptr;
		Ogre::RaySceneQuery* mRaySceneQuery = nullptr;

		OIS::InputManager* mInputManager = nullptr;
		OIS::Keyboard* mKeyboard = nullptr;
		OIS::Mouse* mMouse = nullptr;

		unsigned int mWindowWidth = 0;
		unsigned int mWindowHeight = 0;

		Ogre::Entity*         mOceanSurfaceEnt = nullptr;

		std::map<const VisibleEntity*, Ogre::Entity*> mEntities;
		std::map<const VisibleEntity*, Ogre::ParticleSystem*> mParticleSystems;

		unsigned int mNumEntities = 0;
		unsigned int mNumParticleSystems = 0;

		Constants mConstants;
		const Terrain* mTerrain;

		MouseCursor* mMouseCursor = nullptr;
		InputHandler* mInputHandler = nullptr;
};

#endif


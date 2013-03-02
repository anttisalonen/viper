#ifndef APP_PLANE_H
#define APP_PLANE_H

#include <vector>

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Entity.h"

class PlaneController;
class Missile;

class Plane : public VisibleEntity {
	public:
		Plane(const Common::Vector3& pos);
		virtual void update(float t) override;
		void setTargetVelocity(PrincipalAxis a, float v);
		void setController(PlaneController* p);
		void shoot();
		void addMissile(Missile* m);
		virtual const char* getType() const override;

	private:
		void checkShooting();

		PlaneController* mController = nullptr;
		float mRotationTargetVelocities[3];
		bool mShooting = false;
		std::vector<Missile*> mMissiles;
};

#endif


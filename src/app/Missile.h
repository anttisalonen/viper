#ifndef APP_MISSILE_H
#define APP_MISSILE_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Entity.h"

class Plane;
class Game;

class Missile : public VisibleEntity {
	public:
		Missile(const Plane* plane);
		void update(float t);
		void shoot(Plane* tgt);
		virtual const char* getType() const override;
		void setOffset(const Common::Vector3& offset);
		bool outOfFuel() const;
		bool attached() const;

	private:
		const Plane* mPlane;
		Plane* mTarget = nullptr;
		Common::Vector3 mOffset;
		float mFuelTime;
};

#endif


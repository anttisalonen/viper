#ifndef APP_MISSILE_H
#define APP_MISSILE_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

#include "Entity.h"
#include "Vehicle.h"

class Game;

class Missile : public VisibleEntity {
	public:
		Missile(const Vehicle* owner);
		void update(float t) override;
		void shoot(Vehicle* tgt);
		virtual const char* getType() const override;
		void setOffset(const Common::Vector3& offset);
		bool outOfFuel() const;
		bool attached() const;

	private:
		const Vehicle* mOwner;
		Vehicle* mTarget = nullptr;
		Common::Vector3 mOffset;
		float mFuelTime;
};

#endif


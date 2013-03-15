#ifndef APP_ENTITY_H
#define APP_ENTITY_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

class Entity {
	public:
		virtual ~Entity() { }
};

enum class PrincipalAxis {
	Pitch,
	Yaw,
	Roll,
};

class VisibleEntity : public Entity {
	public:
		VisibleEntity(const Common::Vector3& v,
				const Common::Quaternion& q, float radius);
		virtual void update(float t);
		const Common::Vector3& getPosition() const;
		const Common::Quaternion& getRotation() const;
		const Common::Vector3& getVelocity() const;
		virtual const char* getType() const = 0; // which mesh to use
		bool collidesWith(const VisibleEntity& e) const;
		virtual float getRadius() const; // collision detection

	protected:
		Common::Vector3 mPosition;
		Common::Quaternion mRotation;
		Common::Vector3 mVelocity;
		float mRotationVelocities[3];
		float mRadius;
};


#endif


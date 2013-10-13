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

class PhysicalEntity : public Entity {
	public:
		PhysicalEntity(const Common::Vector3& v,
				const Common::Quaternion& q, float radius);
		virtual void update(float t);
		const Common::Vector3& getPosition() const;
		const Common::Quaternion& getRotation() const;
		void setPosition(const Common::Vector3& v);
		void setVelocity(const Common::Vector3& v);
		void setRotation(const Common::Quaternion& q);
		const Common::Vector3& getVelocity() const;
		bool collidesWith(const PhysicalEntity& e) const;
		virtual float getRadius() const; // collision detection

		// yaw in range [-pi, pi]
		float getYaw() const;

	protected:
		Common::Vector3 mPosition;
		Common::Quaternion mRotation;
		Common::Vector3 mVelocity;
		float mRotationVelocities[3];
		float mRadius;
};

class VisibleEntity : public PhysicalEntity {
	public:
		VisibleEntity(const Common::Vector3& v,
				const Common::Quaternion& q, float radius);
		virtual const char* getType() const = 0; // which mesh to use
};


#endif


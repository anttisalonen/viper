#ifndef APP_ENTITY_H
#define APP_ENTITY_H

#include "common/Vector3.h"
#include "common/Quaternion.h"

class Entity {
};

class VisibleEntity : public Entity {
	public:
		VisibleEntity(const Common::Vector3& v,
				const Common::Quaternion& q);
		const Common::Vector3& getPosition() const;
		const Common::Quaternion& getRotation() const;

	private:
		Common::Vector3 mPosition;
		Common::Quaternion mRotation;
};


#endif


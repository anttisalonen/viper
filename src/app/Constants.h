#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#include <jsoncpp/json/json.h>

#include "common/Vector3.h"

class Constants {
	public:
		Constants(const char* fn);
		Common::Vector3 getVector3(const char* name) const;
		bool getBool(const char* name) const;
		float getFloat(const char* name) const;
		unsigned int getUInt(const char* name) const;
		int getInt(const char* name) const;

	private:
		const Json::Value& getValue(const char* name) const;
		void throwError(const char* error, const char* name) const;

		Json::Value mRoot;
};

#endif


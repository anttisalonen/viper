#include <stdexcept>
#include <fstream>

#include "Constants.h"

Constants::Constants(const char* fn)
{
	Json::Reader reader;

	std::ifstream input(fn, std::ifstream::binary);
	bool parsingSuccessful = reader.parse(input, mRoot, false);
	if (!parsingSuccessful) {
		// report to the user the failure and their locations in the document.
		std::cerr << "Failed to parse constants in file " << fn << "\n";
		throw std::runtime_error(reader.getFormattedErrorMessages());
	}
}

Common::Vector3 Constants::getVector3(const char* name) const
{
	auto val = getValue(name);
	if(!val.isArray() || val.size() != 3) {
		throwError("Is not a valid vector: ", name);
	}
	return Common::Vector3(val[0u].asDouble(), val[1u].asDouble(), val[2u].asDouble());
}

bool Constants::getBool(const char* name) const
{
	return getValue(name).asBool();
}

float Constants::getFloat(const char* name) const
{
	return getValue(name).asDouble();
}

unsigned int Constants::getUInt(const char* name) const
{
	return getValue(name).asUInt();
}

int Constants::getInt(const char* name) const
{
	return getValue(name).asInt();
}

const Json::Value& Constants::getValue(const char* name) const
{
	const Json::Value& val = mRoot[name];
	if(val.isNull()) {
		throwError("No value named", name);
	}
	return val;
}

void Constants::throwError(const char* error, const char* name) const
{
	char buf[128];
	snprintf(buf, 127, "%s '%s'", error, name);
	buf[127] = 0;
	throw std::runtime_error(std::string(buf));
}


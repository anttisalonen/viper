#ifndef APP_TERRAIN_H
#define APP_TERRAIN_H

#include <noise/noise.h>

class Terrain {
	public:
		Terrain(float scale, float offset, float dimension);
		float getHeightAt(float x, float y) const;
		float getDimension() const;

	private:
		noise::module::Perlin mPerlin;
		float mScale;
		float mOffset;
		float mDimension;
};

#endif


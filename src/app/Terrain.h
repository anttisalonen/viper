#ifndef APP_TERRAIN_H
#define APP_TERRAIN_H

#include <vector>

#include <noise/noise.h>

class Terrain {
	public:
		Terrain(float scale, float offset, float dimension);
		float getHeightAt(float x, float y) const;
		float getDimension() const;

	private:
		float getIHeightAt(int ix, int iy) const;

		noise::module::Perlin mPerlin;
		float mScale;
		float mOffset;
		unsigned int mDimension;
		unsigned int mDistance;
		unsigned int mCacheWidth;
		mutable std::vector<float> mHeightCache;
};

#endif


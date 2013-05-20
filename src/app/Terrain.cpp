#include <stdlib.h>
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "Terrain.h"

Terrain::Terrain(float scale, float offset, float dimension)
	: mScale(scale),
	mOffset(offset),
	mDimension(dimension),
	mDistance(16),
	mCacheWidth(mDimension / mDistance)
{
	if(mDimension <= 0) {
		throw std::runtime_error("Invalid terrain dimension");
	}
	unsigned int heightCacheSize = mCacheWidth * mCacheWidth;
	mHeightCache = std::vector<float>(heightCacheSize, 0.0f);
	std::cout << "Terrain: allocating " << heightCacheSize * sizeof(float) << " bytes for cache.\n";
}

// The user input for x, y is expected to be in range (-mDimension, +mDimension).
// We need to:
//  1. Map this to (0, mDimension) => fx, fy
//  2. Quantisize to an integer floored by mDistance, and find the distance => ix, iy, dx, dy
//  3. Find the index by y * cache width + x
float Terrain::getHeightAt(float x, float y) const
{
#if 1
	float fx = x + mDimension / 2;
	float fy = y + mDimension / 2;

	float inx, iny;

	float dx = std::modf(fx / mDistance, &inx);
	float dy = std::modf(fy / mDistance, &iny);

	int ix = (int)inx;
	int iy = (int)iny;

	float h1 = getIHeightAt(ix,     iy);
	float h2 = getIHeightAt(ix + 1, iy);
	float h3 = getIHeightAt(ix,     iy + 1);
	float h4 = getIHeightAt(ix + 1, iy + 1);

	// bilinear filtering
	float a = h1 * (1.0f - dx) * (1.0f - dy);
	float b = h2 * dx * (1.0f - dy);
	float c = h3 * (1.0f - dx) * dy;
	float d = h4 * dx * dy;
	float res = a + b + c + d;
#else
	float res = mOffset + mScale * mPerlin.GetValue(x * 0.001f, y * 0.001f, 0.0f);
#endif

#if 0
	static int cnt = 0;
	if(cnt > 2000) {
		std::cout << x << "\t" << y << "\t" << res << "\n";
	}
	cnt++;
	if(cnt > 2200)
		exit(1);
#endif
	return res;
}

float Terrain::getIHeightAt(int ix, int iy) const
{
	int index = iy * mCacheWidth + ix;
	if(index < 0 || index >= mHeightCache.size()) {
		return 0.0f;
	}

	float& cached = mHeightCache[index];
	if(!cached) {
		cached = mOffset + mScale * mPerlin.GetValue(ix * mDistance * 0.001f, iy * mDistance * 0.001f, 0.0f);
	}
	return cached;
}

float Terrain::getDimension() const
{
	return mDimension;
}


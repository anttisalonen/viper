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

	mBaseFlatTerrain.SetFrequency(2.0);
	mFlatTerrain.SetSourceModule(0, mBaseFlatTerrain);
	mFlatTerrain.SetScale(0.125);
	mFlatTerrain.SetBias(0.0);
	mFinalTerrain.SetSourceModule(0, mFlatTerrain);
	mFinalTerrain.SetSourceModule(1, mMountainTerrain);
	mFinalTerrain.SetControlModule(mTerrainType);
	mFinalTerrain.SetBounds(-1000.0, 0.0);
	mFinalTerrain.SetEdgeFalloff(0.125);
}

// The user input for x, y is expected to be in range (-mDimension, +mDimension).
// We need to:
//  1. Map this to (0, mDimension) => fx, fy
//  2. Quantisize to an integer floored by mDistance, and find the distance => ix, iy, dx, dy
//  3. Find the index by y * cache width + x
float Terrain::getHeightAt(float x, float y) const
{
	for(const auto& mod : mModifiers) {
		if(x >= mod.x1 && x <= mod.x2 && y >= mod.y1 && y <= mod.y2) {
			return mod.h;
		}
	}

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

	return res;
}

float Terrain::getIHeightAt(int ix, int iy) const
{
	int index = iy * mCacheWidth + ix;
	if(index < 0 || index >= mHeightCache.size()) {
		return -1.0f;
	}

	float& cached = mHeightCache[index];
	if(!cached) {
		cached = mOffset + mScale * getRawTerrainHeight(ix * mDistance * 0.001f, iy * mDistance * 0.001f);

#define EDGE_FALL_OFF 10
		if(cached > 1.0) {
			float diff = 1.0;
			if(ix < EDGE_FALL_OFF && ix >= 0) {
				diff = ix;
				cached = cached * diff / float(EDGE_FALL_OFF);
			} else if(ix >= mCacheWidth - EDGE_FALL_OFF - 1 && ix <= mCacheWidth) {
				diff = mCacheWidth - ix + 1;
				cached = cached * diff / float(EDGE_FALL_OFF);
			}

			if(iy < EDGE_FALL_OFF && iy >= 0) {
				diff = iy;
				cached = cached * diff / float(EDGE_FALL_OFF);
			} else if(iy >= mCacheWidth - EDGE_FALL_OFF - 1 && iy <= mCacheWidth) {
				diff = mCacheWidth - iy + 1;
				cached = cached * diff / float(EDGE_FALL_OFF);
			}
			if(cached < 1.0 && cached > -1.0)
				cached = -1.0;
		}
	}
	return cached;
}

float Terrain::getRawTerrainHeight(float x, float y) const
{
	return mFinalTerrain.GetValue(x, y, 0.0f);
}

float Terrain::getDimension() const
{
	return mDimension;
}

void Terrain::addHeightModifier(float x1, float y1, float x2, float y2, float h)
{
	mModifiers.push_back(Modifier(x1, y1, x2, y2, h));
}



#include "Terrain.h"

Terrain::Terrain(float scale, float offset, float dimension)
	: mScale(scale),
	mOffset(offset),
	mDimension(dimension)
{
}

float Terrain::getHeightAt(float x, float y) const
{
	return mOffset + mScale * mPerlin.GetValue(x * 0.001f, y * 0.001f, 0.0f);
}

float Terrain::getDimension() const
{
	return mDimension;
}


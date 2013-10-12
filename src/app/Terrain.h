#ifndef APP_TERRAIN_H
#define APP_TERRAIN_H

#include <vector>

#include <noise/noise.h>

class Terrain {
	public:
		Terrain(float scale, float offset, float dimension);
		float getHeightAt(float x, float y) const;
		float getDimension() const;
		void addHeightModifier(float x1, float y1, float x2, float y2, float h);

	private:
		float getIHeightAt(int ix, int iy) const;

		struct Modifier {
			float x1;
			float y1;
			float x2;
			float y2;
			float h;
			Modifier(float x1_, float y1_, float x2_, float y2_, float h_)
				: x1(x1_),
				y1(y1_),
				x2(x2_),
				y2(y2_),
				h(h_) { }
		};

		noise::module::Perlin mPerlin;
		float mScale;
		float mOffset;
		unsigned int mDimension;
		unsigned int mDistance;
		unsigned int mCacheWidth;
		mutable std::vector<float> mHeightCache;
		std::vector<Modifier> mModifiers;
};

#endif


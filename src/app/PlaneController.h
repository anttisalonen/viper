#ifndef APP_PLANECONTROLLER_H
#define APP_PLANECONTROLLER_H

#include "Plane.h"

class PlaneController {
	public:
		PlaneController(Plane* p = nullptr) : mPlane(p) { }
		virtual ~PlaneController() { }
		void setPlane(Plane* p) { mPlane = p; planeReset(); }
		virtual void update(float t) { }
		virtual void planeReset() { }

	protected:
		Plane* mPlane;
};

#endif


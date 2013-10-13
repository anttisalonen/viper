#ifndef APP_VEHICLECONTROLLER_H
#define APP_VEHICLECONTROLLER_H

#include "Vehicle.h"

class VehicleController {
	public:
		VehicleController(Vehicle* p = nullptr) : mVehicle(p) { }
		virtual ~VehicleController() { }
		void setVehicle(Vehicle* p) { mVehicle = p; vehicleReset(); }
		virtual void update(float t) { }
		virtual void vehicleReset() { }
		const Vehicle* getVehicle() const { return mVehicle; }
		Vehicle* getVehicle() { return mVehicle; }
		virtual int getSide() const = 0;

	protected:
		Vehicle* mVehicle;
};

#endif


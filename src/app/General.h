#ifndef APP_GENERAL_H
#define APP_GENERAL_H

class Game;
class Vehicle;

class General {
	public:
		General(Game* g);
		void update(float time);
		const Vehicle* getSelectedVehicle() const;
		void setSelectedVehicle(Vehicle* v);

	private:
		Game* mGame = nullptr;
		Vehicle* mSelectedVehicle = nullptr;
};

#endif


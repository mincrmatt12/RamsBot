#ifndef Elevator_h
#define Elevator_h
#include "Util/Motor.h"
class Elevator{
	public:
		Elevator(int pin, bool type);
		void moveElevator(int direction);
	private:
		Motor elevator;
};
#endif

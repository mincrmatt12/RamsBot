#ifndef Shooter_h
#define Shooter_h
#include <Arduino.h>
#include "Util/Motor.h"

class Shooter{
	public:
		Shooter(int shooterPinOne, int shooterPinTwo, bool type);
		void setSpeed(int speed);
	private:
		Motor shooterOne, shooterTwo;
};
#endif

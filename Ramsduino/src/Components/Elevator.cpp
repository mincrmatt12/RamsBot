#include <Arduino.h>
#include "Util/Motor.h"

Motor elevator;

Elevator::Elevator(int pin, bool type){
	elevator = Motor(pin, type);
}
void Elevator::moveElevator(int direction){
	int dutyCycle;
	if(direction == 1)
		dutyCycle = 100;
	else if(direction == -1)
		dutyCycle = -100;
	else
		dutyCycle = 0;
	elevator.doPWM(dutyCycle);
}

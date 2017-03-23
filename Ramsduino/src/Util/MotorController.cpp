#include <Arduino.h>
#include <Servo.h>
#include "Util/mathFunc.h"

mathFunc math = mathFunc();
Servo driver;

MotorController::MotorController(bool type, int pin){
	this->type = type;
	driver.attach(pin);
}
void MotorController::setDutyCycle(int dutyCycle){
	int hold[2];
	if(type==true){
		hold[0] = victor.min;
		hold[1] = victor.max;
	}
	else{
		hold[0] = spark.min;
		hold[1] = spark.max;
	}
	driver.writeMicroseconds(math.dutyCycletoMicroSeconds(hold[0],hold[1],dutyCycle));
}

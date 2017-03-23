#include <Arduino.h>
#include <Servo.h>
#include <Stepper.h>
#include "Motor.h"
#include "MotorController.h"
#include "Servo.h"

Stepper stepper = Stepper(0,0,0);

Motor::Motor(){ }
Motor::Motor(int pin, bool type){
	MotorController driver = MotorController(type, pin);
}
Motor::Motor(Stepper _Stepper){
	stepper = _Stepper;
	stepper.setSpeed(200);
}
void Motor::doPWM(int input){
	driver.setDutyCycle(input);
}
void Motor::doStepper(int input){
	stepper.step(input);
}

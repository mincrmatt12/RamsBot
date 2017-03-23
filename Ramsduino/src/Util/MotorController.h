#ifndef MotorController_h
#define MotorController_h
#include <Servo.h>
#include <Arduino.h>
class MotorController{
	public:
		MotorController(bool type, int pin);
		void setDutyCycle(int dutyCycle);
	private:
		struct VICTORSP{
			static const int min = 600;
			static const int max = 2400;
		}victor;
		struct REVSPARK{
			static const int min = 500;
			static const int max = 2500;
		}spark;
		bool type;//false is victor, true is spark
		Servo driver;
};
#endif

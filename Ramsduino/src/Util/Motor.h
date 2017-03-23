#ifndef Motor_h
#define Motor_h
#include "Arduino.h"
#include <Servo.h>
#include <Stepper.h>
#include "MotorController.h"
class Motor{
  public:
    Motor(int pin, bool type);
    Motor(Stepper _Stepper);
    Motor();
    void doStepper(int input);
    void doPWM(int input);
  private:
    MotorController driver;
    Stepper stepper = Stepper(0,0,0);
};
#endif

#include <Arduino.h>
#include "Drive.h"
#include "Util/Motor.h"

Motor leftOne, leftTwo, rightOne, rightTwo;

Drive::Drive(int leftPinOne, int leftPinTwo, int rightPinOne, int rightPinTwo, bool type){
	leftOne = Motor(leftPinOne,type);
	leftTwo = Motor(leftPinTwo,type);
	rightOne = Motor(rightPinOne,type);
	rightTwo = Motor(rightPinTwo,type);
}
void Drive::doDrive(int x, int y){
	int left = x + y;
	int right = x - y;
	leftOne.doPWM((int)(left*100));
	leftTwo.doPWM((int)(left*100));
	rightOne.doPWM((int)(right*(-100)));
	rightTwo.doPWM((int)(right*(-100)));
}

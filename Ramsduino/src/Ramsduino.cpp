#include <Arduino.h>
#include "Constants.h"
//Components
#include "Components/Drive.h"

Constants constant = Constants();

Drive drive = Drive(constant.leftDriveOne,constant.leftDriveTwo,constant.rightDriveOne,constant.rightDriveTwo,constant.victor);

void setup(){

}
void loop(){

}

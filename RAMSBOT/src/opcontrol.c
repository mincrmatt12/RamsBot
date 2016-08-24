/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

//main drive function
void drive(int x,int y){

			//right drive
            motorSet(2,-x);
            motorSet(3,-x);
            //left drive
            motorSet(4,y);
            motorSet(5,y);
            }
/*//Set point in rps?
unsigned long last_time = 0;
unsigned long last_rps = 0;
double sum = 0;
unsigned long count = 0;
unsigned long print_count = 5;


int pid_control(double set_point,double K_P_V,double K_I_V)
{
    const double K_P = K_P_V; //0.027; //0.02 works okay 0.035 is too high
    const double K_I = K_I_V;//0.02;// fine tuning position
    unsigned long K_I_CAP = 2;// maximum I
    const double K_D = 0;// how fast the motor Spins up, can be negative
    const int TICKS_PER_REVOLUTION = 1000;//256;
    int output = 0;
// get rotations
    int rotations = encoderGet(enc)/TICKS_PER_REVOLUTION;
    //how much time has passed
    double time_elapsed = (millis() - last_time)/1000.0;
    //record the previous time
    last_time = millis();
    double current_rps = rotations/time_elapsed; //Currently Rotations per millisecond so if u want you can multiply by 1000
    encoderReset(enc);
    //setpoint is the RPS we hope to achieve (in miliseconds)
    double error = set_point-current_rps;
    output += K_P*(error) + K_I*sum + K_D*(current_rps-last_rps)/time_elapsed; //portion of the error,number of rotations,portion of speed divivded by time
    if( K_I*sum < K_I_CAP )// why?
    {
        sum += (current_rps-last_rps)*time_elapsed;
    }
    else
    {
        sum = 0;
    }
    last_rps = current_rps;

    if( output > 127 )
        output = 127;
    if( output < -127)
        output = -127;

    count++;
    if( count % print_count == 0 )
    {
        printf("Ticks: %d Current RPS: %d Error: %d Output: %d\n", rotations, (int)current_rps, (int)error, output);
        count = 1;
    }
    //delay(50);
    motorSet(7,output);
    motorSet(8,output);
    return error;
}
// find out if shooter is up to speed
    */
int checkShooterStatus(){
	int ticks;
	int enc = encoderGet(enc);
	encoderReset(enc);
	delay(2000);
	ticks = encoderGet(enc);
	int RPS = ticks/2000;
	//remember to comment out the print or it wont work.
	//("%d RPS \n",RPS);
	return RPS;
}
///THE FOLLOWING FOUR FUNCTIONS ARE FOR SHOOTING...

// foul shot code

	int foulShot(int f){
	// make sure that the motor is running at a constant and

		int RPS = checkShooterStatus();
		while (RPS < 32)
			       {
			     	RPS = checkShooterStatus();

			     	printf("%d",f);
			     	 	 motorSet(7,(f));
			     	 	 motorSet(8,(f));
			     	 	 f += 3;
			        }
		while (RPS <=35||RPS>=45 )
		{
			if (RPS > 44)
			{
				f-=2;
				break;
			}
			f += 1;
		  	RPS = checkShooterStatus();
			 motorSet(7,(f));
			 motorSet(8,(f));

			 printf("%d",f);

		}
		while (RPS <=40||RPS>=45 )
			{
				if (RPS > 44)
				{
					f-=2;
					break;
				}
				f += 1;
			  	RPS = checkShooterStatus();
				 motorSet(7,(f));
				 motorSet(8,(f));

				 printf("%d",f);

			}
		return f;
	}
	// tell eleveator/driver that the shooter is ready for a foul shot

int threePoint (int f){

	int RPS = checkShooterStatus();
		while (RPS < 28)
			       {
			     	RPS = checkShooterStatus();

			     	printf("%d",f);
			     	 	 motorSet(7,(f));
			     	 	 motorSet(8,(f));
			     	 	 f += 3;
			        }
		while (RPS <=30||RPS>=48 )
		{
			if (RPS > 47)
			{
				f-=2;
				break;
			}
			f += 1;
		  	RPS = checkShooterStatus();
			 motorSet(7,(f));
			 motorSet(8,(f));

			 printf("%d",f);

		}
		while (RPS <47||RPS>=48 )
			{
				if (RPS > 47)
				{
					f-=3;
				}
				else
				f += 1;
			  	RPS = checkShooterStatus();
				 motorSet(7,(f));
				 motorSet(8,(f));

				 printf("%d",f);

			}
		return f;
	}
	// tell eleveator/driver that the shooter is about to shoot a three

//full court shot
int halfCourt (){
// make sure that the motor is running at a constant and
	int RPS = checkShooterStatus();
		int f = 70;
		while (RPS < 70)
			       {
			if(f>= 127)
			{
						     		f = 127;
			}
			     	RPS = checkShooterStatus();

			     	//printf("%d",f);
			     	 	 motorSet(7,(f));
			     	 	 motorSet(8,(f));
			     	 	 f += 5;
			        }
		while (RPS <75)
		{
			if(f>= 127)
			{
				f = 127;
			}
		  	RPS = checkShooterStatus();
			 motorSet(7,(f));
			 motorSet(8,(f));
			 f += 2;

		}
	// tell eleveator/driver that the shooter is ready to shoot a halfcourt shot
	return f;
}
// will compare the elevator to the speed of the shooter before the shot and then
// move the ball into the shooter using the elevator.
// elevator will stop once the ball is launched because the encoder speed will decrease to below the
// shot speed.
void elevator(int shot){
		while(checkShooterStatus() >= shot ){
				motorSet(6,-127);
			}
		motorStop(6);
		}
// in manual override if the user wishes to raise or lower the voltage values for the motors
int increment(bool x, int z){
	if (x == false)
	{z-=25;}
	else if(x== true)
	{z+=25;}

return z;

}

void operatorControl() {
	int f,x,y;

	while (1) {
		delay(20);
			if(5>joystickGetAnalog(1,2)>-5){
		          x = joystickGetAnalog(1,2);}
			if(5>joystickGetAnalog(1,3)>-5){
		          y = joystickGetAnalog(1,3);}

			drive(x,y);

		        while (joystickGetDigital(1, 8, JOY_RIGHT) == 1)
		        {
		            motorSet(6,127); //conveyor DOWN
		        }

		        while (joystickGetDigital(1, 8, JOY_DOWN) == 1)
		        {
		                    motorSet(6,-127); //conveyor UP
		        }
		        motorStop(6);
		        if (joystickGetDigital(1, 8, JOY_LEFT) == 1)
		        {
		        		   f = 78;
		        		   motorSet(7,f); // shooter at full voltage
		        		   motorSet(8,f);
		        		            // manual increment depending on the joystick value
		        }


		        if (joystickGetDigital(1, 8, JOY_UP) == 1)
		        {
		        		   f = 127;
		        		   motorSet(7,30); // shooter at full speed
		        		   motorSet(8,30); //in order to score a foul

		        }

		        if (joystickGetDigital(1,6,JOY_UP) == 1){
		        		/*if (f<=102){
		        		       f=	increment(true,f);
		        		       motorSet(7,15); // shooter at full speed
		        		       motorSet(8,15);
		        		 // }*/

		        }
		        if (joystickGetDigital(1,6,JOY_DOWN) == 1){
		           	if (f>=25){
		           			f =increment(false,f);
		           			motorSet(7,f); // shooter at full speed
		           			motorSet(8,f);
		           			}
		        }

		        if (joystickGetDigital(1,7,JOY_DOWN)==1){
		        	motorStop(7);
		        	motorStop(8);
		        }
		        if (joystickGetDigital(1,7,JOY_LEFT)==1)
		        {
		        	f = 40;
		       		        	f = foulShot(f);
		        }

		        if (joystickGetDigital(1,7,JOY_UP)==1){
		        	f = 50;
		       		        	f = threePoint(f);}
		        }}



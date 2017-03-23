#ifndef Constants_h
#define Constants_h
class Constants{
	public:
		Constants(){};
		//Motor Controllers
		static const bool victor = true;
		static const bool spark = false;
		//Motor pin declarations
		static const int leftDriveOne = 0;
		static const int leftDriveTwo = 1;
		static const int rightDriveOne = 2;
		static const int rightDriveTwo = 3;
		static const int shooterOne = 4;
		static const int shooterTwo = 5;
		static const int elevatorOne = 6;
};
#endif

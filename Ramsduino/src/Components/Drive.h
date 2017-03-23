#ifndef Drive_h
#define Drive_h
#include "Util/Motor.h"
class Drive{
	public:
		Drive(int leftPinOne, int leftPinTwo, int rightPinOne, int rightPinTwo, bool type);
		void doDrive(int x, int y);
	private:
		Motor leftOne, leftTwo, rightOne, rightTwo;
};
#endif

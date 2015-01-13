#ifndef _bot_h
#define _bot_h

namespace mrvision {

class Bot{

	int mId;
	double mX, mY, mAngle;

public:
	Bot();
	Bot( int aId, double aX, double aY, double aAngle );

	int getId();

	double getX();
	double getY();

	double getAngle();
private:

};


}

#endif // _bot_h

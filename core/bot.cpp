#include "bot.h"

namespace mrvision {

Bot::Bot() :
		mId(),
		mX(),
		mY(),
		mAngle()
{

}

Bot::Bot( int aId, double aX, double aY, double aAngle ) :
		mId(aId),
		mX(aX),
		mY(aY),
		mAngle(aAngle)
{

}

int Bot::getId(){
	return mId;
}

double Bot::getX(){
	return mX;
}
double Bot::getY(){
	return mY;
}

double Bot::getAngle(){
	return mAngle;
}

}

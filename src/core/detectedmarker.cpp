#include "src/core/detectorsimple.h"

DetectedMarker::DetectedMarker( int aMarkerID, int aXPosition, int aYPosition, vector<Vector2D>* aCorners = new vector<Vector2D>() ) :
    mMarkerID(aMarkerID),
    mXPosition(aXPosition),
    mYPosition(aYPosition)
{
    mCorners = aCorners;
};

DetectedMarker~DetectedMarker(){

    delete mCorners;

};

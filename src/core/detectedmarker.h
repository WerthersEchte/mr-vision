#ifndef _detectedmarker_h
#define _detectedmarker_h

#include <string>

namespace mrvision {

class DetectedMarker {

    int mMarkerID;
    int mXPosition, mYPosition;
    float mDirection;

    vector<Vector2D>* mCorners;

public:
    DetectedMarker( int aMarkerID, int aXPosition, int aYPosition, vector<Vector2D>* aCorners = new vector<Vector2D>() );
    virtual ~DetectedMarker();

};

}

#endif // _detectedmarker_h

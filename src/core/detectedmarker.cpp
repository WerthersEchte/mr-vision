#include "src/core/detectedmarker.h"

namespace mrvision {

DetectedMarker::DetectedMarker( cv::Mat aMarkerImage, float aMarkerDirection, cv::Point aMarkerPosition ) :
    mMarkerImage(aMarkerImage),
    mMarkerDirection(aMarkerDirection),
    mMarkerPosition(aMarkerPosition),
    mMarker(),
    mMarkerId(-1),
	mMarkerConfidence(0.0)
{

}

DetectedMarker::~DetectedMarker(){

};

}

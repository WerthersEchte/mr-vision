#ifndef _detectedmarker_h
#define _detectedmarker_h

#include <string>

#include "opencv2/opencv.hpp"

namespace mrvision {

class DetectedMarker {

public:
    cv::Mat mMarkerImage;
    float mMarkerDirection;
    cv::Point mMarkerPosition;


    std::vector<bool> mMarker;
	float mMarkerConfidence;
    int mMarkerId;

    DetectedMarker( cv::Mat aMarkerImage, float aMarkerDirection, cv::Point aMarkerPosition );

    ~DetectedMarker();


};

}

#endif // _detectedmarker_h

#ifndef _detectorsimple_h
#define _detectorsimple_h

#include "src/core/detector.h"
#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <string>

namespace mrvision {

static MarkerList MARKERLIST;

class DetectorSimple : public Detector{

    aruco::CameraParameters mCameraParameters;
    aruco::MarkerDetector mDetector;
    float mSizeOfMarker;
    int mThreshold;

public:
    DetectorSimple( std::string aCameraParameterFile, float aSizeOfMarker = 0.025 );
    virtual ~DetectorSimple();

    virtual void setMarkerList( MarkerList* aMarkerList );
    virtual std::vector<aruco::Marker>* detectMarkers( const cv::Mat& aImage );

    void setThreshold( int aThreshold );

private:

    static int identifyMarker( const cv::Mat &in,int &nRotations );

};

}

#endif // _detectorsimple_h

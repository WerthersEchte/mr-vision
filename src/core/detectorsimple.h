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
    bool isRunning;

public:
    DetectorSimple( std::string aCameraParameterFile, float aSizeOfMarker = 0.025 );
    virtual ~DetectorSimple();

    virtual void setMarkerList( MarkerList* aMarkerList );

public slots:
    virtual void detectMarkers( const cv::Mat& aImage );

private:
    void detectingMarkerInImage( const cv::Mat& aImage );
    static int identifyMarker( const cv::Mat &in,int &nRotations );

};

}

#endif // _detectorsimple_h

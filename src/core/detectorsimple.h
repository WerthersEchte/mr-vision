#ifndef _detectorsimple_h
#define _detectorsimple_h

#include "src/core/detector.h"
#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include "ill/MarkerDetector.h"

#include <string>

namespace mrvision {

static MarkerList MARKERLIST;

class DetectorSimple : public Detector{

    Q_OBJECT

    aruco::CameraParameters mCameraParameters;
    double mThreshold;
    int mMinSize, mMaxSize;
    bool isRunning, mStatus;

public:
    DetectorSimple( float aSizeOfMarker = 0.025 );
    virtual ~DetectorSimple();

    virtual void setParameter( int aParameter, void* aValue );
    virtual void getParameter( int aParameter, void* aValue );
    virtual void setStatus( bool aStatus );

public slots:
    virtual void detectMarkers( const cv::Mat& aImage );

    void blahMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker );

private:
    void detectingMarkerInImage( const cv::Mat& aImage );
    static int identifyMarker( const cv::Mat &in,int &nRotations );

};

}

#endif // _detectorsimple_h

#ifndef _detectorsimple_h
#define _detectorsimple_h

#include "src/core/detector.h"
#include "src/core/detectedmarker.h"
#include "src/core/markerlist.h"
#include "src/core/bot.h"

#include <string>
#include <vector>

#include "opencv2/opencv.hpp"

namespace mrvision {

static MarkerList MARKERLIST;

class DetectorSimple : public Detector{

    Q_OBJECT

    int mThreshold;
    int mMinSize, mMaxSize;
    bool isRunning, mStatus;

    cv::Mat mBinaryImage;
    std::vector<cv::Point> mPoints, mArea, mRealPoints;
    int mCounterI, mCounterJ, mCounterK, mCounterL;

    int mMaxRangeBetweenPoints, mMinRangeBetweenPoints;
    std::vector<std::vector<int> > mDistanceBetweenPoints, mFoundCombinations;

    int mPointA, mPointB, mPointC, mPointD;
    int mDistanceAB, mDistanceBC, mDistanceCD, mDistanceAD, mDistanceAC, mDistanceBD;
    bool mDistanceABtest, mDistanceBCtest, mDistanceCDtest, mDistanceADtest, mDistanceACtest, mDistanceBDtest;

public:
    DetectorSimple( float aSizeOfMarker = 0.025 );
    virtual ~DetectorSimple();

    virtual void setParameter( parameter aParameter, void* aValue );
    virtual void getParameter( parameter aParameter, void* aValue );
    virtual void setStatus( bool aStatus );

public slots:
    virtual void detectMarkers( const cv::Mat& aImage );

    void blahMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker );

private:
    bool checkForDuplicates( int vA, int vB, int vC, int vD );
    bool checkAngles( const cv::Point& aA, const cv::Point& aB, const cv::Point& aC, const cv::Point& aD );

    void detectingMarkerInImage( const cv::Mat& aImage );
    bool identifyMarker( DetectedMarker& aNotYetFoundMarker );

};

}

#endif // _detectorsimple_h

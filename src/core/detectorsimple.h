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


	int mIteration;

    int mThreshold;
    int mMinSize, mMaxSize;
    int mDetectionAreaMaxX, mDetectionAreaMaxY, mDetectionAreaMinX, mDetectionAreaMinY;
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
    // Read CameraFile - maybe move to Camera
    cv::Mat mCameraMatrix;
    cv::Mat mDistortionMatrix;
    cv::Size mCameraSize;

public:
    DetectorSimple( float aSizeOfMarker = 0.025, int aSizeOfImageX = 800, int aSizeOfImageY = 600 );
    virtual ~DetectorSimple();
    virtual void setStatus( bool aStatus );

    virtual void setCameraFile( QString aCameraFileName );
    virtual void setThreshold( int aThreshold );
    virtual int getThreshold();
    void setDetectionArea( int aMinX, int aMaxX, int aMinY, int aMaxY );

public slots:
    virtual void detectMarkers( const cv::Mat& aImage );

private:
    bool checkForDuplicates( int vA, int vB, int vC, int vD );
    bool checkAngles( const cv::Point& aA, const cv::Point& aB, const cv::Point& aC, const cv::Point& aD );

    void detectingMarkerInImage( const cv::Mat& aImage );
    bool identifyMarker( DetectedMarker& aNotYetFoundMarker );

};

}

#endif // _detectorsimple_h

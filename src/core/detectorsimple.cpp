#include "src/core/detectorsimple.h"

#include "src/core/detectedmarker.h"

#include "iostream"

#include <QtConcurrent/QtConcurrent>
#include "opencv2/opencv.hpp"

namespace mrvision {

DetectorSimple::DetectorSimple( float aSizeOfMarker ) :
    mThreshold(0.3),
    mMinSize(3),
    mMaxSize(10),
    isRunning(false),
    mStatus(false),
    mCameraParameters()
{

    qRegisterMetaType< cv::Mat >("cv::Mat");

    connect( &mDetector, SIGNAL( showMarkerAndImage( cv::Mat, QList<bool> ) ), this, SLOT( blahMarkerAndImage( cv::Mat, QList<bool> ) ) );

}

DetectorSimple::~DetectorSimple(){}

aruco::CameraParameters* DetectorSimple::getCameraFile(){

    return &mCameraParameters;

}

void DetectorSimple::detectingMarkerInImage( const cv::Mat& aImage ){

    std::vector<DetectedMarker> vFoundMarkers;

    // TODO: please do the needful for this image

    cv::Mat vBinaryImage( aImage.size(), aImage.type() );
    cv::threshold( aImage, vBinaryImage, mThreshold, 1, cv::THRESH_BINARY );

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    cv::findContours( vBinaryImage, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    for( vector<Point> vRegion : contours ){

        for( )


    }


    if( vFoundMarkers.size() ){
        //emit markersDetected( vFoundMarkers );
    }
    isRunning = false;

}

void DetectorSimple::detectMarkers( const cv::Mat& aImage ){

    if( mStatus && !isRunning ){
        isRunning = true;
        QtConcurrent::run(this, &DetectorSimple::detectingMarkerInImage, aImage);
    }

}

int DetectorSimple::identifyMarker( const cv::Mat &in, int &nRotations ){

    cv::Mat vBWMarker = in > 128;

    cv::Mat vPart;
	nRotations = 0;
    int width=vBWMarker.size().width / 5,
    	height=vBWMarker.size().height / 5;
    std::vector<bool> vMarker;
    for( int i = 1; i < 4; i++){
    	for( int j = 1; j < 4; j++){
            vPart = cv::Mat(vBWMarker, cv::Rect( j * width, i * height, width, height));
    		vMarker.push_back( countNonZero(vPart)>width*height/2 );
    	}
    }

    for( mrvision::Marker vTest : MARKERLIST.getMarker() ){
		if( vTest.compareTo( vMarker, nRotations ) ){
			return vTest.getId();
		}
    }

    return -1;
}

void DetectorSimple::setParameter( int aParameter, void* aValue ){

}

void DetectorSimple::getParameter( int aParameter, void* aValue ){

}

void DetectorSimple::blahMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker ){
    emit showMarkerAndImage( aImage, aMarker );
}

void DetectorSimple::setStatus( bool aStatus ){

    mStatus = aStatus;

}
}

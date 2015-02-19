#include "src/core/detectorsimple.h"

#include "iostream"

#include <QtConcurrent/QtConcurrent>
#include "opencv2/opencv.hpp"

namespace mrvision {

DetectorSimple::DetectorSimple( float aSizeOfMarker ) :
    mDetector(),
    mSizeOfMarker(aSizeOfMarker),
    isRunning(false),
    mStatus(false),
    mCameraParameters()
{

    mDetector.setMinMaxSize(0.001);
    mDetector.setMakerDetectorFunction(&identifyMarker);

    qRegisterMetaType< cv::Mat >("cv::Mat");

    connect( &mDetector, SIGNAL( showMarkerAndImage( cv::Mat, QList<bool> ) ), this, SLOT( blahMarkerAndImage( cv::Mat, QList<bool> ) ) );

}

DetectorSimple::~DetectorSimple(){}

void DetectorSimple::setMarkerSize( float aMarkerSize ){

    mSizeOfMarker = aMarkerSize;

}

float DetectorSimple::getMarkerSize(){

    return mSizeOfMarker;

}

void DetectorSimple::setCameraFile( QString aCameraFile ){

    try{
        mCameraParameters.readFromXMLFile( aCameraFile.toLocal8Bit().constData() );
    } catch( const cv::Exception& vException ){

        std::cout << vException.what() << std::endl;

    }
}

aruco::CameraParameters* DetectorSimple::getCameraFile(){

    return &mCameraParameters;

}

void DetectorSimple::detectingMarkerInImage( const cv::Mat& aImage ){

    std::vector<aruco::Marker> vFoundMarkers;

    if( mCameraParameters.isValid() ){
        mCameraParameters.resize( aImage.size() );
        mDetector.detect( aImage , vFoundMarkers, mCameraParameters, mSizeOfMarker );

    }

    if( vFoundMarkers.size() ){
        emit markersDetected( vFoundMarkers );
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

void DetectorSimple::setThreshold( int aThreshold ){

    mDetector.THRESHOLD = aThreshold;

}

int DetectorSimple::getThreshold(){

    return mDetector.THRESHOLD;

}

void DetectorSimple::blahMarkerAndImage( const cv::Mat& aImage, const QList<bool>& aMarker ){
    emit showMarkerAndImage( aImage, aMarker );
}

void DetectorSimple::setStatus( bool aStatus ){

    mStatus = aStatus;

}
}

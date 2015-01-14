#include "src/core/detectorsimple.h"

#include "iostream"

namespace mrvision {

DetectorSimple::DetectorSimple( std::string aCameraParameterFile, float aSizeOfMarker ) :
    mDetector(),
    mSizeOfMarker(aSizeOfMarker)
{
    mCameraParameters.readFromXMLFile( aCameraParameterFile.c_str() );

    mDetector.setMinMaxSize(0.001);
    mDetector.setMakerDetectorFunction(&identifyMarker);
}

DetectorSimple::~DetectorSimple(){}

void DetectorSimple::setMarkerList( MarkerList* aMarkerList ){

    //DetectorSimple::mMarkerList = aMarkerList;

}

std::vector<aruco::Marker>* DetectorSimple::detectMarkers( const cv::Mat& aImage ){

    std::vector<aruco::Marker>* vFoundMarkers = new std::vector<aruco::Marker>();

    mCameraParameters.resize( aImage.size() );
    mDetector.detect(aImage ,*vFoundMarkers, mCameraParameters, mSizeOfMarker);

    return vFoundMarkers;

}

void DetectorSimple::setThreshold( int aThreshold ){

    mThreshold = aThreshold;

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

}

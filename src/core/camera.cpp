#include "camera.h"

#include <stdio.h>
#include "src/core/udp_server.h"
#include <QtConcurrent/QtConcurrent>

#include <iostream>

namespace mrvision {

Camera::Camera(DShowLib::Grabber::tVideoCaptureDeviceItem aCameraIdent) :
	mCameraId(aCameraIdent)
{
	mCamera.openDev(aCameraIdent);
	mCamera.setVideoFormat("Y800 (640x480)");
	mCamera.setOverlayBitmapPathPosition(DShowLib::ePP_NONE);
	mCamera.setFPS(60.0);

	DShowLib::Grabber::tVidFmtListPtr vTest = mCamera.getAvailableVideoFormats();
	for (int vI = 0;vI < vTest.get()->size() ;++vI) {
		std::cout << vTest.get()->at(vI).c_str() << std::endl;
	}

	std::cout << mCamera.getFPS() << " " << mCamera.getFrameRate() << " " << mCamera.getVideoFormat().c_str() << std::endl;

	mSink = DShowLib::FrameHandlerSink::create(DShowLib::eY800, 1);
	mSink->setSnapMode(true);
	mCamera.setSinkType(mSink);
	
	if (!mCamera.prepareLive(false))
	{
		std::cerr << "Could not render the VideoFormat into a eY800 sink.";
	}

	mSink->getOutputFrameType(mInfo);

	mBuffer[0] = new BYTE[mInfo.buffersize];

	// Create a new MemBuffer collection that uses our own image buffers.
	DShowLib::tMemBufferCollectionPtr pCollection = DShowLib::MemBufferCollection::create(mInfo, 1, mBuffer);
	if (pCollection == 0 || !mSink->setMemBufferCollection(pCollection))
	{
		std::cerr << "Could not set the new MemBufferCollection, because types do not match." << mCamera.getLastError() << std::endl;
	}

}

Camera::~Camera(){

    if( isValid() ){
        while( isRunning() ){
            mIsFetchingVideoframes = false;
        }
		mCamera.closeDev();
    }

}

void Camera::startVideoCapture(){

    if( isValid() ){
        start();
    }

}

void Camera::stopVideoCapture(){

    mIsFetchingVideoframes = false;

}

void Camera::getShutterMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_SHUTTER, aMin, aMax );

}
void Camera::getGainMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_GAIN, aMin, aMax );

}
void Camera::getGammaMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_GAMMA, aMin, aMax );

}
void Camera::getSharpnessMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_SHARPNESS, aMin, aMax );

}
/*
void Camera::getFeatureMinMax( dc1394feature_t aFeature, unsigned int* aMin, unsigned int* aMax ){

    if( isValid() ){
        //dc1394_feature_get_boundaries( mCamera, aFeature, aMin, aMax );
    }

}
*/
void Camera::setShutterMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            //dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL);
        } else {
            //dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
        }
    }

}

void Camera::setShutter( int aShutterTime ){

    if( isValid() ){

        //dc1394_feature_set_value(mCamera, DC1394_FEATURE_SHUTTER, aShutterTime);

    }

}

int Camera::getShutter(){

    unsigned int vValue = 0;
    if( isValid() ){
        //dc1394_feature_get_value(mCamera, DC1394_FEATURE_SHUTTER, &vValue);

    }
    return vValue;

}

void Camera::setGainMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            //dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
        } else {
            //dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
        }
    }

}

void Camera::setGain( int aGain ){

    if( isValid() ){

       // dc1394_feature_set_value(mCamera, DC1394_FEATURE_GAIN, aGain );

    }

}

int Camera::getGain(){

    unsigned int vValue = 0;
    if( isValid() ){
       // dc1394_feature_get_value(mCamera, DC1394_FEATURE_GAIN, &vValue);

    }
    return vValue;

}


void Camera::setGammaMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
           // dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_MANUAL);
        } else {
           // dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_AUTO);
        }
    }

}
void Camera::setGamma( int aGamma ){

    if( isValid() ){

        //dc1394_feature_set_value(mCamera, DC1394_FEATURE_GAMMA, aGamma );

    }

}

int Camera::getGamma(){

    unsigned int vValue = 0;
    if( isValid() ){
        //dc1394_feature_get_value(mCamera, DC1394_FEATURE_GAMMA, &vValue);

    }
    return vValue;

}


void Camera::setSharpnessMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
           // dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHARPNESS, DC1394_FEATURE_MODE_MANUAL);
        } else {
           // dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHARPNESS, DC1394_FEATURE_MODE_AUTO);
        }
    }

}
void Camera::setSharpness( int aSharpness ){

    if( isValid() ){

        //dc1394_feature_set_value(mCamera, DC1394_FEATURE_SHARPNESS, aSharpness );

    }

}

int Camera::getSharpness(){

    unsigned int vValue = 0;
    if( isValid() ){
       // dc1394_feature_get_value(mCamera, DC1394_FEATURE_SHARPNESS, &vValue);

    }
    return vValue;

}

bool Camera::isValid(){

	return mCamera.isDevOpen() && mCamera.isDevValid();

}

std::string Camera::getId(){

	return mCameraId.getName();

}

void Camera::decodeBotPositions( const std::vector<DetectedMarker>& aListOfMarkers ){

    QtConcurrent::run(this, &Camera::calculateBotPositions, aListOfMarkers);

}

void Camera::calculateBotPositions( const std::vector<DetectedMarker>& aListOfMarkers ){

    QList<mrvision::Bot> vBots;

    for( DetectedMarker vMarker : aListOfMarkers )
    {
        vBots.append( mrvision::Bot(
                vMarker.mMarkerId,
                ((float)(mULMultiplier*(mUpperX - mLowerX)) + vMarker.mMarkerPosition.y - mLowerX) / (mUpperX - mLowerX !=0 ? (2*(mUpperX - mLowerX)) : 1) ,
                ((float)(mLRMultiplier*(mUpperY - mLowerY)) + vMarker.mMarkerPosition.x - mLowerY) / (mUpperY - mLowerY !=0 ? ((mUpperY - mLowerY)) : 1),
                vMarker.mMarkerDirection*-1+90) ); //TODO: better
        emit detectedBot(vMarker.mMarkerId);
    }

    emit decodedBotPositions( vBots );

}

QMutex *Camera::mMutex = new QMutex();

void Camera::run(){

    mIsFetchingVideoframes = true;
	cv::Mat vCurrentFrame = cv::Mat(mInfo.dim.cy, mInfo.dim.cx, CV_8UC1);

	bool vTookFrame = false;

	mCamera.startLive(false);

    while( mIsFetchingVideoframes ){

		mSink->snapImages(1);

		memcpy(vCurrentFrame.ptr(), mBuffer[0], mInfo.buffersize);
		emit newVideoFrame(vCurrentFrame);

    }

	mCamera.suspendLive();
	mCamera.stopLive();
}

QList<Camera*> Camera::findCameras(){

    QList<Camera*> vCameraList;
	
	DShowLib::InitLibrary();
	DShowLib::Grabber vGrabber;
	DShowLib::Grabber::tVidCapDevListPtr vListOfDevices = vGrabber.getAvailableVideoCaptureDevices();

	for (int vI = 0; vI < vListOfDevices.get()->size(); ++vI) {
		vCameraList.append(new Camera(vListOfDevices.get()->at(vI)));
	}

    return vCameraList;
	
}

}

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
	DShowLib::tsPropertyRange vRange = mCamera.getPropertyRange(DShowLib::Grabber::tCameraPropertyEnum::CameraControl_Exposure);
	*aMin = vRange.min;
	*aMax = vRange.max;
}
void Camera::getGainMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_GAIN, aMin, aMax );
	DShowLib::tsPropertyRange vRange = mCamera.getPropertyRange(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain);
	*aMin = vRange.min;
	*aMax = vRange.max;

}
void Camera::getGammaMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_GAMMA, aMin, aMax );
	DShowLib::tsPropertyRange vRange = mCamera.getPropertyRange(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gamma);
	*aMin = vRange.min;
	*aMax = vRange.max;

}
void Camera::getSharpnessMinMax( unsigned int* aMin, unsigned int* aMax ){

    //getFeatureMinMax( DC1394_FEATURE_SHARPNESS, aMin, aMax );
	DShowLib::tsPropertyRange vRange = mCamera.getPropertyRange(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Sharpness);
	*aMin = vRange.min;
	*aMax = vRange.max;

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
		mCamera.setProperty(DShowLib::Grabber::tCameraPropertyEnum::CameraControl_Exposure, !aManual);
    }

}

void Camera::setShutter( int aShutterTime ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain, false);
		mCamera.setProperty(DShowLib::Grabber::tCameraPropertyEnum::CameraControl_Exposure, static_cast<long>(aShutterTime));

    }

}

int Camera::getShutter(){

    unsigned int vValue = 0;
    if( isValid() ){
		vValue = mCamera.getProperty(DShowLib::Grabber::tCameraPropertyEnum::CameraControl_Exposure);
    }
    return vValue;

}

void Camera::setGainMode( bool aManual ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain, !aManual);
    }

}

void Camera::setGain( int aGain ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain, false);
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain, static_cast<long>(aGain));
    }

}

int Camera::getGain(){

    unsigned int vValue = 0;
    if( isValid() ){
		vValue = mCamera.getProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gain);
    }
    return vValue;

}


void Camera::setGammaMode( bool aManual ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gamma, !aManual);
    }

}
void Camera::setGamma( int aGamma ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gamma, false);
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gamma, static_cast<long>(aGamma));
    }

}

int Camera::getGamma(){

    unsigned int vValue = 0;
    if( isValid() ){
		vValue = mCamera.getProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Gamma);
    }
    return vValue;

}


void Camera::setSharpnessMode( bool aManual ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Sharpness, !aManual);
    }

}
void Camera::setSharpness( int aSharpness ){

    if( isValid() ){
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Sharpness, false);
		mCamera.setProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Sharpness, static_cast<long>(aSharpness));
    }

}

int Camera::getSharpness(){

    unsigned int vValue = 0;
    if( isValid() ){
		vValue = mCamera.getProperty(DShowLib::Grabber::tVideoPropertyEnum::VideoProcAmp_Sharpness);
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
		vBots.append(mrvision::Bot(
			vMarker.mMarkerId,
			(((static_cast<double>(vMarker.mMarkerPosition.x) - mLowerXCamera) / (mUpperXCamera - mLowerXCamera) * (mUpperXPlayfield - mLowerXPlayfield)) + mLowerXPlayfield),
			(((static_cast<double>(vMarker.mMarkerPosition.y) - mLowerYCamera) / (mUpperYCamera - mLowerYCamera) * (mUpperYPlayfield - mLowerYPlayfield)) + mLowerYPlayfield),
            vMarker.mMarkerDirection*-1 )); //TODO: better
        emit detectedBot(vMarker.mMarkerId);

		std::cout << "Position:  " << vMarker.mMarkerPosition.x << " | " << vMarker.mMarkerPosition.y << " Bot: " << vBots.last().getX() << " | " << vBots.last().getY() << std::endl;
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

#include "camera.h"

#include <stdio.h>
#include "src/core/udp_server.h"
#include <QtConcurrent/QtConcurrent>

#include <iostream>

namespace mrvision {

Camera::Camera( dc1394camera_t *aCamera ) :
        mCamera(aCamera),
        mLastVideoframe(nullptr),
        mIsFetchingVideoframes(false)
{

    if( !aCamera ){
        return;
    }

    //TODO: need to check for errors

    dc1394_video_set_iso_speed(mCamera, DC1394_ISO_SPEED_400);

    dc1394_video_set_mode(mCamera, DC1394_VIDEO_MODE_800x600_MONO8);

    dc1394_video_set_framerate(mCamera, DC1394_FRAMERATE_30);

    dc1394_capture_setup(mCamera, 4, DC1394_CAPTURE_FLAGS_DEFAULT);

    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_WHITE_BALANCE, DC1394_FEATURE_MODE_AUTO);
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_AUTO);
    dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHARPNESS, DC1394_FEATURE_MODE_AUTO);

    dc1394_video_set_transmission(mCamera, DC1394_ON);

    dc1394_get_image_size_from_video_mode(mCamera, DC1394_VIDEO_MODE_800x600_MONO8, &mCameraImageWidth, &mCameraImageHeight);

}

Camera::~Camera(){

    if( isValid() ){
        while( isRunning() ){
            mIsFetchingVideoframes = false;
        }
        dc1394_video_set_transmission(mCamera, DC1394_OFF);
        dc1394_capture_stop(mCamera);
        dc1394_camera_free(mCamera);

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

    getFeatureMinMax( DC1394_FEATURE_SHUTTER, aMin, aMax );

}
void Camera::getGainMinMax( unsigned int* aMin, unsigned int* aMax ){

    getFeatureMinMax( DC1394_FEATURE_GAIN, aMin, aMax );

}
void Camera::getGammaMinMax( unsigned int* aMin, unsigned int* aMax ){

    getFeatureMinMax( DC1394_FEATURE_GAMMA, aMin, aMax );

}
void Camera::getSharpnessMinMax( unsigned int* aMin, unsigned int* aMax ){

    getFeatureMinMax( DC1394_FEATURE_SHARPNESS, aMin, aMax );

}

void Camera::getFeatureMinMax( dc1394feature_t aFeature, unsigned int* aMin, unsigned int* aMax ){

    if( isValid() ){
        dc1394_feature_get_boundaries( mCamera, aFeature, aMin, aMax );
    }

}

void Camera::setShutterMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL);
        } else {
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
        }
    }

}

void Camera::setShutter( int aShutterTime ){

    if( isValid() ){

        dc1394_feature_set_value(mCamera, DC1394_FEATURE_SHUTTER, aShutterTime);

    }

}

int Camera::getShutter(){

    unsigned int vValue = 0;
    if( isValid() ){
        dc1394_feature_get_value(mCamera, DC1394_FEATURE_SHUTTER, &vValue);

    }
    return vValue;

}

void Camera::setGainMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
        } else {
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
        }
    }

}

void Camera::setGain( int aGain ){

    if( isValid() ){

        dc1394_feature_set_value(mCamera, DC1394_FEATURE_GAIN, aGain );

    }

}

int Camera::getGain(){

    unsigned int vValue = 0;
    if( isValid() ){
        dc1394_feature_get_value(mCamera, DC1394_FEATURE_GAIN, &vValue);

    }
    return vValue;

}


void Camera::setGammaMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_MANUAL);
        } else {
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_GAMMA, DC1394_FEATURE_MODE_AUTO);
        }
    }

}
void Camera::setGamma( int aGamma ){

    if( isValid() ){

        dc1394_feature_set_value(mCamera, DC1394_FEATURE_GAMMA, aGamma );

    }

}

int Camera::getGamma(){

    unsigned int vValue = 0;
    if( isValid() ){
        dc1394_feature_get_value(mCamera, DC1394_FEATURE_GAMMA, &vValue);

    }
    return vValue;

}


void Camera::setSharpnessMode( bool aManual ){

    if( isValid() ){
        if( aManual ){
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHARPNESS, DC1394_FEATURE_MODE_MANUAL);
        } else {
            dc1394_feature_set_mode(mCamera, DC1394_FEATURE_SHARPNESS, DC1394_FEATURE_MODE_AUTO);
        }
    }

}
void Camera::setSharpness( int aSharpness ){

    if( isValid() ){

        dc1394_feature_set_value(mCamera, DC1394_FEATURE_SHARPNESS, aSharpness );

    }

}

int Camera::getSharpness(){

    unsigned int vValue = 0;
    if( isValid() ){
        dc1394_feature_get_value(mCamera, DC1394_FEATURE_SHARPNESS, &vValue);

    }
    return vValue;

}

bool Camera::isValid(){

    return mCamera!=nullptr;

}

uint64_t Camera::getId(){

    return mCamera->guid;

}

void Camera::decodeBotPositions( const std::vector<aruco::Marker>& aListOfMarkers ){

    QtConcurrent::run(this, &Camera::calculateBotPositions, aListOfMarkers);

}

#define PI 3.14159265

void Camera::calculateBotPositions( const std::vector<aruco::Marker>& aListOfMarkers ){

    QList<mrvision::Bot> vBots;
    int k = 11,it, offset=16, sosize=706;

    for( aruco::Marker vMarker : aListOfMarkers )
    {
        cv::Point2f vPoint = vMarker[1] - vMarker[2];
        vBots.append( mrvision::Bot(
                vMarker.id,
                ((mULMultiplier*(mUpperX - mLowerX)) + vMarker.getCenter().y - mLowerX) / (2*(mUpperX - mLowerX)),
                ((mLRMultiplier*(mUpperY - mLowerY)) + vMarker.getCenter().x - mLowerY) / (mUpperY - mLowerY),
                atan2( vPoint.x, vPoint.y) * 180 / PI) );
        emit detectedBot(vMarker.id);
    }

    emit decodedBotPositions( vBots );

}

void Camera::run(){

    mIsFetchingVideoframes = true;

    while( mIsFetchingVideoframes ){

        dc1394_capture_dequeue(mCamera, DC1394_CAPTURE_POLICY_WAIT, &mLastVideoframe);
        cv::Mat vCurrentFrame( mCameraImageHeight, mCameraImageWidth, CV_8UC1, mLastVideoframe->image );
        dc1394_capture_enqueue( mCamera, mLastVideoframe);

        emit newVideoFrame( vCurrentFrame );

    }

}

QList<Camera*> Camera::findCameras(){

    QList<Camera*> vCameraList;

    dc1394_t *d;
    dc1394camera_list_t *list;
    dc1394error_t err;

    d = dc1394_new();
    if ( d ){

        err = dc1394_camera_enumerate( d, &list );
        if( err != DC1394_SUCCESS){

            dc1394_log_error( "Failed to enumerate cameras (Errorcode %d)", err);
            return vCameraList;

        }

        for( int i = 0; i < list->num; i++){

            vCameraList.append( new Camera( dc1394_camera_new ( d, list->ids[i].guid) ) );

            if( !vCameraList.empty() && !vCameraList.last() ){

                vCameraList.removeLast();
                dc1394_log_error( "Failed to initialize camera with guid %llx", list->ids[i].guid);

            }

        }

    }
    dc1394_camera_free_list (list);

    return vCameraList;

}

}

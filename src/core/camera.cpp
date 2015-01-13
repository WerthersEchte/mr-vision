#include "camera.h"

namespace mrvision {

Camera::Camera( dc1394camera_t *aCamera ) :
        mCamera(aCamera),
        mLastVideoframe(nullptr)
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

    dc1394_video_set_transmission(mCamera, DC1394_ON);

    dc1394_get_image_size_from_video_mode(mCamera, DC1394_VIDEO_MODE_800x600_MONO8, &mCameraImageWidth, &mCameraImageHeight);

}

Camera::~Camera(){

    if( isValid() ){

        dc1394_video_set_transmission(mCamera, DC1394_OFF);
        dc1394_capture_stop(mCamera);
        dc1394_camera_free(mCamera);

    }

}

cv::Mat* Camera::getVideoFrame(){

    cv::Mat* vCurrentFrame = nullptr;

    if( isValid() ){

        //TODO: Errorchecks
        dc1394_capture_dequeue(mCamera, DC1394_CAPTURE_POLICY_WAIT, &mLastVideoframe);
        vCurrentFrame = new cv::Mat( mCameraImageHeight, mCameraImageWidth, CV_8UC1, mLastVideoframe->image );
        dc1394_capture_enqueue( mCamera, mLastVideoframe);

    }

    return vCurrentFrame;

}

bool Camera::isValid(){

    return mCamera!=nullptr;

}

uint64_t Camera::getId(){

    return mCamera->guid;

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
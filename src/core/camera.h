#ifndef _camera_h
#define _camera_h

#include <dc1394/dc1394.h>
#include <QList>
#include "opencv2/opencv.hpp"

#include <QMutex>

namespace mrvision {

class Camera{

    mutable QMutex mMutex;

    dc1394camera_t *mCamera;
    dc1394video_frame_t *mLastVideoframe;

    unsigned int mCameraImageHeight, mCameraImageWidth;

    cv::Mat* mCurrentPicture;
    bool mIsFetchingPictures;

public:
    Camera( int aId );
	Camera( dc1394camera_t *aCamera );
    ~Camera();

    cv::Mat* getVideoFrame();

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();

private:

    void getPictureLoop();

};


}

#endif // _camera_h

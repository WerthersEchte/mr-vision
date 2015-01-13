#ifndef _camera_h
#define _camera_h

#include <dc1394/dc1394.h>
#include <QList>
#include "opencv2/opencv.hpp"

namespace mrvision {

class Camera{

    dc1394camera_t *mCamera;
    dc1394video_frame_t *mLastVideoframe;

    unsigned int mCameraImageHeight, mCameraImageWidth;

public:
    Camera( int aId );
	Camera( dc1394camera_t *aCamera );
    ~Camera();

    cv::Mat* getVideoFrame();

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();
private:

};


}

#endif // _camera_h

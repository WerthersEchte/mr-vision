#ifndef _camera_h
#define _camera_h

#include <dc1394/dc1394.h>
#include <QList>
#include "opencv2/opencv.hpp"

#include <QThread>

namespace mrvision {

class Camera: public QThread {

    Q_OBJECT

    dc1394camera_t *mCamera;
    dc1394video_frame_t *mLastVideoframe;

    unsigned int mCameraImageHeight, mCameraImageWidth;

    bool mIsFetchingVideoframes;

public:
    Camera( int aId );
	Camera( dc1394camera_t *aCamera );
    ~Camera();

    void startVideoCapture();
    void stopVideoCapture();

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();

signals:
    void newVideoFrame( const cv::Mat& aPicture );

private:
    void run();

};


}

#endif // _camera_h

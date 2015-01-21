#ifndef _camera_h
#define _camera_h

#include "src/core/bot.h"

#include <dc1394/dc1394.h>
#include <QList>
#include "opencv2/opencv.hpp"
#include <aruco/aruco.h>

#include <QThread>

namespace mrvision {

class Camera: public QThread {

    Q_OBJECT

    dc1394camera_t *mCamera;
    dc1394video_frame_t *mLastVideoframe;

    unsigned int mCameraImageHeight, mCameraImageWidth;

    bool mIsFetchingVideoframes;

public:
    int mLowerX, mLowerY, mUpperX, mUpperY, mULMultiplier, mLRMultiplier;

    Camera( int aId );
	Camera( dc1394camera_t *aCamera );
    ~Camera();

    void startVideoCapture();
    void stopVideoCapture();

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();

public slots:
    void decodeBotPositions( const std::vector<aruco::Marker>& aListOfMarkers );

signals:
    void newVideoFrame( const cv::Mat& aPicture );
    void decodedBotPositions( QList<mrvision::Bot> aBots );

private:
    void run();
    void calculateBotPositions( const std::vector<aruco::Marker>& aListOfMarkers );

};


}

#endif // _camera_h

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

    int getShutter();
    int getGain();
    int getGamma();

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();

public slots:
    void decodeBotPositions( const std::vector<aruco::Marker>& aListOfMarkers );

    void setShutterMode( bool aManual = true );
    void setShutter( int aShutterTime = 400 );
    void setGainMode( bool aManual = true );
    void setGain( int aGain = 400 );
    void setGammaMode( bool aManual = true );
    void setGamma( int aGain = 400 );

signals:
    void newVideoFrame( const cv::Mat& aPicture );
    void decodedBotPositions( QList<mrvision::Bot> aBots );
    void detectedBot( int aId );

private:
    void run();
    void calculateBotPositions( const std::vector<aruco::Marker>& aListOfMarkers );

};


}

#endif // _camera_h

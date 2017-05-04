#ifndef _camera_h
#define _camera_h

#include "src/core/bot.h"
#include "src/core/detectedmarker.h"

#include <dc1394/dc1394.h>
#include <dc1394/control.h>
#include <QList>
#include "opencv2/opencv.hpp"
//#include <aruco/aruco.h>

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
    int getSharpness();

    void getShutterMinMax( unsigned int* aMin, unsigned int* aMax );
    void getGainMinMax( unsigned int* aMin, unsigned int* aMax );
    void getGammaMinMax( unsigned int* aMin, unsigned int* aMax );
    void getSharpnessMinMax( unsigned int* aMin, unsigned int* aMax );

    bool isValid();

    uint64_t getId();

    static QList<Camera*> findCameras();

public slots:
    void decodeBotPositions( const std::vector<DetectedMarker>& aListOfMarkers );

    void setShutterMode( bool aManual = true );
    void setShutter( int aShutterTime = 400 );
    void setGainMode( bool aManual = true );
    void setGain( int aGain = 400 );
    void setGammaMode( bool aManual = true );
    void setGamma( int aGamma = 400 );
    void setSharpnessMode( bool aManual = true );
    void setSharpness( int aSharpness = 400 );

signals:
    void newVideoFrame( const cv::Mat& aPicture );
    void decodedBotPositions( QList<mrvision::Bot> aBots );
    void detectedBot( int aId );

private:
    void run();
    void calculateBotPositions( const std::vector<DetectedMarker>& aListOfMarkers );

    void getFeatureMinMax( dc1394feature_t aFeature, unsigned int* aMin, unsigned int* aMax );

};


}

#endif // _camera_h

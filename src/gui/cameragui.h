#ifndef CAMERAGUI_H
#define CAMERAGUI_H

#include <QWidget>
#include <QPixmap>
#include <QTableWidgetItem>
#include <QList>

#include "src/core/camera.h"
#include "src/core/detector.h"
#include "src/core/detectorsimple.h"

namespace Ui {
	class Camera;
}

namespace mrvision{

class MarkerGui;

class CameraGui : public QWidget
{
	Q_OBJECT

	Ui::Camera *mUi;
	Camera* mCamera;
	DetectorSimple* mDetector;

	QList<MarkerGui*> mKnownMarkers;
	std::vector<DetectedMarker> mListOfMarkers;

	bool mActiveInterface, mTakeScreenshot;

public:
    explicit CameraGui( Camera *aCamera, QWidget *aParent = 0 );
    ~CameraGui();

private slots:
    void showInterface( bool aDummy );

    void takeScreenshot( bool aScreenshot = false );
    void detectBots( bool aActivateDetection = false );
    void startStreamingVideo( bool aStreaming = false );
    void startStreamingMarker( bool aStreaming = false );

    void showMarker( bool aShow = false );
    void showDetection( bool aShow = false );
    void showCameraData( bool aShow = false );

    void paintPicture(const QPixmap &aPicture);
    void paintMarkerPicture(const QPixmap &aMarkerPic,const QPixmap &aFoundMarker);

    void setCameraFile();
    void setMarkerSize();

    void selectCameraFile( bool aDummy );
    void setlEThreshold( int aValue );
    void setThreshold( );

    void setBorderData();

    void setlEShutter( int aValue );
    void setShutter( );
    void setlEGain( int aValue );
    void setGain( );
    void setlEGamma( int aValue );
    void setGamma( );
    void setlESharpness( int aValue );
    void setSharpness( );

    void saveCameraGuiConfig( bool aDummy );
    void loadCameraGuiConfig( bool aDummy );

public slots:
    void streamVideo( const cv::Mat& aVideoFrame );
    void streamFoundMarkers( const cv::Mat& aImage, const QList<bool>& aMarker );
    void connectLiveMarker( bool aChecked );
    void saveCurrentMarkers( const std::vector<DetectedMarker>& aListOfMarkers );

signals:
    void newPicture( const QPixmap & );
    void newMarkerPixs(const QPixmap &aMarkerPic,const QPixmap &aFoundMarker);

private:
    void createPictureFromVideoframe( const cv::Mat& aVideoFrame );
    void createMarkerPictures( const cv::Mat& aImage, const QList<bool>& aMarker );

};

}

#endif // CAMERAGUI_H

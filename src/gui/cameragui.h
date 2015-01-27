#ifndef CAMERAGUI_H
#define CAMERAGUI_H

#include <QWidget>
#include <QPixmap>
#include <QTableWidgetItem>
#include <QList>

#include "src/core/camera.h"
#include "src/core/detector.h"

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
	Detector* mDetector;

	QList<MarkerGui*> mKnownMarkers;

public:
    explicit CameraGui( Camera *aCamera, QWidget *aParent = 0 );
    ~CameraGui();

private slots:
    void detectBots( bool aActivateDetection = false );
    void startStreamingVideo( bool aStreaming = false );
    void showMarker( bool aShow = false );

    void paintPicture(const QPixmap &aPicture);

    void setCameraFile();
    void setMarkerSize();

    void selectCameraFile( bool aDummy );

    void pushUp( bool aDummy );
    void pushDown( bool aDummy );
    void pushLeft( bool aDummy );
    void pushRight( bool aDummy );

    void setBorderData();

public slots:
    void streamVideo( const cv::Mat& aVideoFrame );

signals:
    void newPicture( const QPixmap & );

private:
    void createPictureFromVideoframe( const cv::Mat& aVideoFrame );

};

}

#endif // CAMERAGUI_H

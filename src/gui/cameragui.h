#ifndef CAMERAGUI_H
#define CAMERAGUI_H

#include <QWidget>
#include <QPixmap>
#include "src/core/camera.h"
#include "src/core/detector.h"

namespace Ui {
	class Camera;
}

namespace mrvision{

class CameraGui : public QWidget
{
	Q_OBJECT

	Ui::Camera *mUi;
	Camera* mCamera;
	Detector* mDetector;

public:
    explicit CameraGui( Camera *aCamera, QWidget *aParent = 0 );
    ~CameraGui();

private slots:
    void detectBots( bool aActivateDetection = false );
    void startStreamingVideo( bool aStreaming = false );

    void paintPicture(const QPixmap &aPicture);

    void testMarkerDetection( std::vector<aruco::Marker> aListOfMarkers );

public slots:
    void streamVideo( const cv::Mat& aVideoFrame );

signals:
    void newPicture( const QPixmap & );

private:
    void createPictureFromVideoframe( const cv::Mat& aVideoFrame );

};

}

#endif // CAMERAGUI_H

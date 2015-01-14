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

	bool mDetectionIsRunning, mStreamIsRunning;

public:
    explicit CameraGui( Camera *aCamera, QWidget *aParent = 0 );
    ~CameraGui();
public slots:
    void detectBots( bool aActivateDetection = false );
    void streamVideo( bool aActivateStream = false );
    void paintPicture(const QPixmap &aPicture);

signals:
    void newPicture(const QPixmap &);

private:
    void streamLoop();
    void detectionLoop();

};

}

#endif // CAMERAGUI_H

#ifndef VISIONGUI_H
#define VISIONGUI_H

#include <QMainWindow>

#include <list>

namespace Ui {
	class Vision;
}

namespace mrvision {

class CameraGui;

class VisionGui : public QMainWindow
{
	Q_OBJECT

	Ui::Vision *mUi;
	std::list<mrvision::CameraGui*> mCameraGuis;

public:
        explicit VisionGui( QWidget *aParent = 0 );
        ~VisionGui();
public slots:
        void findCameras( bool aDummy = false );

};

}

#endif // VISIONGUI_H

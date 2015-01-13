#include "visiongui.h"
#include "ui_vision.h"

#include "cameragui.h"
#include "src/core/camera.h"

#include <QScrollArea>

#include <cstdlib> // random
#include <ctime> // time for random


namespace mrvision {

VisionGui::VisionGui( QWidget *aParent) :
    QMainWindow(aParent),
    mUi(new Ui::Vision),
    mCameraGuis()
{
    mUi->setupUi(this);
    connect( mUi->pBFindCameras, SIGNAL( clicked( bool ) ), this, SLOT( findCameras( bool ) ) );

    mUi->gBListofCameras->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

VisionGui::~VisionGui() {
    delete mUi;
}

void VisionGui::findCameras( bool aDummy ) {

    for(mrvision::CameraGui* vCamera : mCameraGuis ){
        delete vCamera;
    }
    mCameraGuis.clear();
    mUi->gBListofCameras->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mUi->gBListofCameras->layout()->setSizeConstraint(QLayout::SetFixedSize);

    foreach( Camera* vCamera, Camera::findCameras() ){

        mrvision::CameraGui* vCameraGui = new mrvision::CameraGui( vCamera );
        static_cast<QBoxLayout*>(mUi->gBListofCameras->layout())->insertWidget( 0, vCameraGui);
        vCameraGui->show();
        mCameraGuis.push_back(vCameraGui);

    }

}

}

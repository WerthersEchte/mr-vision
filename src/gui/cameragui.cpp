#include "cameragui.h"
#include "ui_camera.h"

#include <QThread>
#include <QtConcurrent/QtConcurrent>

namespace mrvision {

CameraGui::CameraGui( Camera *aCamera, QWidget *aParent) :
    QWidget(aParent),
    mUi(new Ui::Camera),
    mCamera(aCamera),
    mStreamIsRunning(false)
{
    mUi->setupUi(this);
    connect( mUi->grpBCameraStream, SIGNAL( clicked( bool ) ), this, SLOT( streamVideo( bool ) ) );
    connect( this, SIGNAL( newPicture( QPixmap ) ), this, SLOT( paintPicture( QPixmap ) ) );
}

CameraGui::~CameraGui() {

    while( mStreamIsRunning ){
        mUi->grpBCameraStream->setChecked( false );
    }
    delete mUi;
    delete mCamera;
}

void CameraGui::streamVideo( bool aActivateStream ){

    if( aActivateStream && !mStreamIsRunning ){

        QtConcurrent::run(this, &CameraGui::streamLoop);

    }

}

void CameraGui::paintPicture(const QPixmap &aPicture){

    mUi->lblVideoStream->setPixmap(aPicture);

}

void CameraGui::streamLoop(){

    const uchar *qImageBuffer;
    cv::Mat *mat;

    QVector<QRgb> vColorTable;
    for (int i=0; i<256; i++){
        vColorTable.push_back(qRgb(i,i,i));
    }
    mStreamIsRunning = true;
    while( mUi->grpBCameraStream->isChecked()){

        // Copy input Mat
        mat = mCamera->getVideoFrame();
        qImageBuffer = (const uchar*)mat->data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat->cols, mat->rows, mat->step, QImage::Format_Indexed8);
        img.setColorTable(vColorTable);

        emit newPicture(QPixmap::fromImage(img));

    }

    mStreamIsRunning = false;
}

}

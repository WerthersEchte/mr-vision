#include "cameragui.h"
#include "ui_camera.h"

#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>

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
    QPixmap vPixmap;

    QPen LinePenRed(Qt::red,1), LinePenGreen(Qt::green,1);

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

        vPixmap = QPixmap::fromImage(img);

        if( mUi->cBGridlines->isChecked() ){

            QPainter painter(&vPixmap);
            painter.setPen(LinePenRed);

            for( int i = 10; i < vPixmap.height(); i = i + 10){
                if( !(i%50) ){
                    painter.setPen(LinePenGreen);
                    painter.drawLine(0,i,vPixmap.width(),i);
                    painter.setPen(LinePenRed);
                } else {
                    painter.drawLine(0,i,vPixmap.width(),i);
                }
            }
            for( int i = 10; i < vPixmap.width(); i = i + 10){
                if( !(i%50) ){
                    painter.setPen(LinePenGreen);
                    painter.drawLine(i,0,i,vPixmap.height());
                    painter.setPen(LinePenRed);
                } else {
                    painter.drawLine(i,0,i,vPixmap.height());
                }
            }

            painter.end();

        }

        emit newPicture(vPixmap);

    }

    mStreamIsRunning = false;
}

}

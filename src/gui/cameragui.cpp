#include "cameragui.h"
#include "ui_camera.h"

#include "src/core/detectorsimple.h"
#include "src/core/udp_server.h"

#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>

#include <aruco/aruco.h>

#include <iostream>

namespace mrvision {

CameraGui::CameraGui( Camera *aCamera, QWidget *aParent) :
    QWidget(aParent),
    mUi(new Ui::Camera),
    mCamera(aCamera),
    mDetector(new DetectorSimple( "cameralinks.yml" ))
{
    mUi->setupUi(this);
    connect( mUi->cBActive, SIGNAL( clicked( bool ) ), this, SLOT( detectBots( bool ) ) );
    connect( mUi->grpBCameraStream, SIGNAL( clicked( bool ) ), this, SLOT( startStreamingVideo( bool ) ) );
    connect( this, SIGNAL( newPicture( QPixmap ) ), this, SLOT( paintPicture( QPixmap ) ) );

    mDetector->setMarkerList( new MarkerList() );

    mUi->lblName->setText( QString::number( mCamera->getId() ) );

    UDPServer::getInstance()->startServer();

    qRegisterMetaType< std::vector<aruco::Marker> >("std::vector<aruco::Marker>");
    connect( mDetector, SIGNAL( markersDetected( std::vector<aruco::Marker> ) ), mCamera, SLOT( decodeBotPositions( std::vector<aruco::Marker> ) ) );
    qRegisterMetaType< QList<mrvision::Bot> >("QList<mrvision::Bot>");
    connect( mCamera, SIGNAL( decodedBotPositions( QList<mrvision::Bot> ) ), UDPServer::getInstance(), SLOT( send_Data( QList<mrvision::Bot> ) ) );

}

CameraGui::~CameraGui() {

    delete mUi;
    delete mCamera;
    delete mDetector;

}

void CameraGui::detectBots( bool aActivateDetection ){

    if( aActivateDetection ){

        qRegisterMetaType< cv::Mat >("cv::Mat");
        connect( mCamera, SIGNAL( newVideoFrame( cv::Mat ) ), mDetector, SLOT( detectMarkers( cv::Mat ) ) );

    } else {

        disconnect( mCamera, &Camera::newVideoFrame, mDetector, &Detector::detectMarkers );

    }

}

void CameraGui::startStreamingVideo( bool aStreaming ){

    if( aStreaming ){

        qRegisterMetaType< cv::Mat >("cv::Mat");
        connect( mCamera, SIGNAL( newVideoFrame( cv::Mat ) ), this, SLOT( streamVideo( cv::Mat ) ) );

        if( !mCamera->isRunning() ){

            mCamera->startVideoCapture();
        }

    } else {

        disconnect( mCamera, &Camera::newVideoFrame, this, &CameraGui::streamVideo );

    }

}


void CameraGui::streamVideo( const cv::Mat& aVideoFrame ){

    QtConcurrent::run(this, &CameraGui::createPictureFromVideoframe, aVideoFrame);

}

void CameraGui::createPictureFromVideoframe( const cv::Mat& aVideoFrame ){

    if( mUi->grpBCameraStream->isChecked() ){

        QPixmap vPixmap;

        QPen LinePenRed(Qt::red,1), LinePenGreen(Qt::green,1);

        QVector<QRgb> vColorTable;
        for (int i=0; i<256; i++){
            vColorTable.push_back(qRgb(i,i,i));
        }

        // Copy input Mat
        // Create QImage with same dimensions as input Mat
        QImage img((const uchar*)aVideoFrame.data, aVideoFrame.cols, aVideoFrame.rows, aVideoFrame.step, QImage::Format_Indexed8);
        img.setColorTable(vColorTable);

        vPixmap = QPixmap::fromImage( img );

        if( mUi->cBGridlines->isChecked() ){

            QPainter painter( &vPixmap );
            painter.setPen( LinePenRed );

            for( int i = 10; i < vPixmap.height(); i = i + 10){
                if( (i%50) ){
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
            painter.setPen( LinePenGreen );
            for( int i = 50; i < vPixmap.height(); i = i + 50){
                painter.drawLine(0,i,vPixmap.width(),i);
            }

            painter.end();

        }

        emit newPicture(vPixmap);

    }

}

void CameraGui::paintPicture(const QPixmap &aPicture){

    mUi->lblVideoStream->setPixmap(aPicture);

}

}

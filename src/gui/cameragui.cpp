#include "cameragui.h"
#include "ui_camera.h"

#include "src/core/detectorsimple.h"
#include "src/core/udp_server.h"
#include "src/core/marker.h"
#include "src/gui/markergui.h"
#include "src/gui/flowlayout.h"

#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QValidator>
#include <QJsonObject>

#include <aruco/aruco.h>

#include <iostream>
#include <limits>

namespace mrvision {

CameraGui::CameraGui( Camera *aCamera, QWidget *aParent) :
    QWidget(aParent),
    mUi(new Ui::Camera),
    mCamera(aCamera),
    mDetector(new DetectorSimple()),
    mKnownMarkers(),
    mActiveInterface(true)
{
    mUi->setupUi(this);

    mUi->lblName->setText( QString::number( mCamera->getId() ) );
    connect( mUi->pBActivateControls, SIGNAL( clicked( bool ) ), this, SLOT( showInterface( bool ) ) );

    mUi->lEMarkerSize->setText( QString::number( mDetector->getMarkerSize() ) );

    mUi->wCont2->hide();
    mUi->wCont3->hide();

    QValidator *vValidator = new QIntValidator(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), this);

    mUi->lELeftBorder->setValidator( vValidator );
    mUi->lERightBorder->setValidator( vValidator );
    mUi->lEUpperBorder->setValidator( vValidator );
    mUi->lELowerBorder->setValidator( vValidator );
    mUi->lEULOffset->setValidator( vValidator );
    mUi->lELROffset->setValidator( vValidator );
    mUi->lEPointsperClick->setValidator( vValidator );

    mUi->lEShutter->setValidator( vValidator );

    vValidator = new QDoubleValidator( 0.0, 100.0, 5, this);
    mUi->lEMarkerSize->setValidator( vValidator );

    mUi->hSShutter->setValue( mCamera->getShutter() );
    mUi->hSShutter->setTracking( false );
    mUi->lEShutter->setText( QString::number(mCamera->getShutter()) );
    connect( mUi->gBShutter, SIGNAL( clicked( bool ) ), mCamera, SLOT( setShutterMode( bool ) ) );
    connect( mUi->hSShutter, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setShutter( int ) ) );
    connect( mUi->hSShutter, SIGNAL( valueChanged( int ) ), this, SLOT( setlEShutter( int ) ) );
    connect( mUi->lEShutter, SIGNAL( editingFinished( ) ), this, SLOT( setShutter( ) ) );

    mUi->hSGain->setValue( mCamera->getGain() );
    mUi->hSGain->setTracking( false );
    mUi->lEGain->setText( QString::number(mCamera->getGain()) );
    connect( mUi->gBGain, SIGNAL( clicked( bool ) ), mCamera, SLOT( setGainMode( bool ) ) );
    connect( mUi->hSGain, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setGain( int ) ) );
    connect( mUi->hSGain, SIGNAL( valueChanged( int ) ), this, SLOT( setlEGain( int ) ) );
    connect( mUi->lEGain, SIGNAL( editingFinished( ) ), this, SLOT( setGain( ) ) );

    mUi->hSGamma->setValue( mCamera->getGamma() );
    mUi->hSGamma->setTracking( false );
    mUi->lEGamma->setText( QString::number(mCamera->getGamma()) );
    connect( mUi->gBGamma, SIGNAL( clicked( bool ) ), mCamera, SLOT( setGammaMode( bool ) ) );
    connect( mUi->hSGamma, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setGamma( int ) ) );
    connect( mUi->hSGamma, SIGNAL( valueChanged( int ) ), this, SLOT( setlEGamma( int ) ) );
    connect( mUi->lEGamma, SIGNAL( editingFinished( ) ), this, SLOT( setGamma( ) ) );

    connect( mUi->cBActive, SIGNAL( clicked( bool ) ), this, SLOT( detectBots( bool ) ) );
    connect( mUi->gBMarker, SIGNAL( clicked( bool ) ), this, SLOT( showMarker( bool ) ) );
    connect( mUi->gBDetection, SIGNAL( clicked( bool ) ), this, SLOT( showDetection( bool ) ) );
    connect( mUi->gBCameraData, SIGNAL( clicked( bool ) ), this, SLOT( showCameraData( bool ) ) );
    connect( mUi->gBCameraStream, SIGNAL( clicked( bool ) ), this, SLOT( startStreamingVideo( bool ) ) );
    connect( this, SIGNAL( newPicture( QPixmap ) ), this, SLOT( paintPicture( QPixmap ) ) );

    qRegisterMetaType< std::vector<aruco::Marker> >("std::vector<aruco::Marker>");
    connect( mDetector, SIGNAL( markersDetected( std::vector<aruco::Marker> ) ), mCamera, SLOT( decodeBotPositions( std::vector<aruco::Marker> ) ) );
    qRegisterMetaType< QList<mrvision::Bot> >("QList<mrvision::Bot>");
    connect( mCamera, SIGNAL( decodedBotPositions( QList<mrvision::Bot> ) ), UDPServer::getInstance(), SLOT( send_Data( QList<mrvision::Bot> ) ) );

    connect( mUi->lEMarkerSize, SIGNAL( editingFinished( ) ), this, SLOT( setMarkerSize( ) ) );

    connect( mUi->lECameraFile, SIGNAL( editingFinished( ) ), this, SLOT( setCameraFile( ) ) );
    connect( mUi->pBLoadCameraConfigFile, SIGNAL( clicked( bool ) ), this, SLOT( selectCameraFile( bool ) ) );

    connect( mUi->pBMoveUp, SIGNAL( clicked( bool ) ), this, SLOT( pushUp( bool ) ) );
    connect( mUi->pBMoveDown, SIGNAL( clicked( bool ) ), this, SLOT( pushDown( bool ) ) );
    connect( mUi->pBMoveLeft, SIGNAL( clicked( bool ) ), this, SLOT( pushLeft( bool ) ) );
    connect( mUi->pBMoveRight, SIGNAL( clicked( bool ) ), this, SLOT( pushRight( bool ) ) );

    connect( mUi->lELeftBorder, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );
    connect( mUi->lERightBorder, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );
    connect( mUi->lEUpperBorder, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );
    connect( mUi->lELowerBorder, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );
    connect( mUi->lEULOffset, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );
    connect( mUi->lELROffset, SIGNAL( editingFinished( ) ), this, SLOT( setBorderData( ) ) );

    MarkerList vMarkerList;
    int vPosition = 0;
    FlowLayout *vLayout = new FlowLayout();

    foreach( Marker vMarker, vMarkerList.getMarker() ){

        MarkerGui* vMarkerGui = new MarkerGui( vMarker );
        vLayout->addWidget( vMarkerGui );
        vMarkerGui->hide();
        mKnownMarkers.append( vMarkerGui );
        connect( mCamera, SIGNAL( detectedBot( int ) ), vMarkerGui, SLOT( blink( int ) ) );

    }
    mUi->gBMarker->setLayout( vLayout );

    connect(mUi->pBSaveCameraConfig, SIGNAL( clicked( bool ) ), this, SLOT( saveCameraGuiConfig( bool ) ) );

}

CameraGui::~CameraGui() {

    foreach( MarkerGui* vMarkerGui, mKnownMarkers ){

        delete vMarkerGui;

    }

    delete mUi;
    delete mCamera;
    delete mDetector;

}

void CameraGui::saveCameraGuiConfig( bool aDummy )
{
    QFile saveFile( QFileDialog::getSaveFileName(this, tr("Select CameraGuiConfig"), "", tr("CameraGuiConfig Files (*.json);;All Files (*.*)")));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject vCameraConfigData;

    vCameraConfigData["CameraGUID"] = static_cast<qint64>(mCamera->getId());

    QJsonDocument saveDoc(vCameraConfigData);
    saveFile.write(saveDoc.toJson());

}

void CameraGui::loadCameraGuiConfig( bool aDummy )
{

}

void CameraGui::setlEShutter( int aValue ){

    mUi->lEShutter->setText( QString::number( aValue ) );

}

void CameraGui::setShutter( ){

    mCamera->setShutter( mUi->lEShutter->text().toInt() );

}

void CameraGui::setlEGain( int aValue ){

    mUi->lEGain->setText( QString::number( aValue ) );

}

void CameraGui::setGain( ){

    mCamera->setGain( mUi->lEGain->text().toInt() );

}

void CameraGui::setlEGamma( int aValue ){

    mUi->lEGamma->setText( QString::number( aValue ) );

}

void CameraGui::setGamma( ){

    mCamera->setGamma( mUi->lEGamma->text().toInt() );

}

void CameraGui::showInterface( bool aDummy ){

    if( mActiveInterface ){

        mActiveInterface = false;
        mUi->gBDetection->hide();
        mUi->gBMarker->hide();
        mUi->gBCameraData->hide();
        mUi->gBCameraStream->hide();

    } else {

        mActiveInterface = true;
        mUi->gBDetection->show();
        mUi->gBMarker->show();
        mUi->gBCameraData->show();
        mUi->gBCameraStream->show();

    }

}

void CameraGui::showDetection( bool aShow ){

    if( aShow ){

        mUi->wCont->show();

    } else {

        mUi->wCont->hide();

    }

}

void CameraGui::showCameraData( bool aShow ){

    if( aShow ){

        mUi->gBBorders->show();
        mUi->gBFeatures->show();

    } else {

        mUi->gBBorders->hide();
        mUi->gBFeatures->hide();

    }

}

void CameraGui::showMarker( bool aShow ){

    if( aShow ){

        foreach( MarkerGui* vMarkerGui, mKnownMarkers ){

            vMarkerGui->show();

        }

    } else {

        foreach( MarkerGui* vMarkerGui, mKnownMarkers ){

            vMarkerGui->hide();

        }
    }

}


void CameraGui::setCameraFile( ){

    if( QFile::exists( mUi->lECameraFile->text() ) ){
        mDetector->setCameraFile( mUi->lECameraFile->text() );
    }

}

void CameraGui::setMarkerSize( ){

    mDetector->setMarkerSize( mUi->lEMarkerSize->text().toFloat() );

}

void CameraGui::selectCameraFile( bool aDummy ){

    QString vCameraFileName = QFileDialog::getOpenFileName(this, tr("Select Camerafile"), "", tr("Camera Files (*.yml);;All Files (*.*)"));

	if ( vCameraFileName.isEmpty() ){
		return;
	}

    mUi->lECameraFile->setText( vCameraFileName );
    setCameraFile();

}

void CameraGui::detectBots( bool aActivateDetection ){

    if( aActivateDetection ){

        qRegisterMetaType< cv::Mat >("cv::Mat");
        connect( mCamera, SIGNAL( newVideoFrame( cv::Mat ) ), mDetector, SLOT( detectMarkers( cv::Mat ) ) );

        if( !mCamera->isRunning() ){

            mCamera->startVideoCapture();
        }

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

        mUi->wCont2->show();
        mUi->wCont3->show();

    } else {

        disconnect( mCamera, &Camera::newVideoFrame, this, &CameraGui::streamVideo );

        mUi->wCont2->hide();
        mUi->wCont3->hide();

    }

}

void CameraGui::streamVideo( const cv::Mat& aVideoFrame ){

    QtConcurrent::run(this, &CameraGui::createPictureFromVideoframe, aVideoFrame);

}

void CameraGui::createPictureFromVideoframe( const cv::Mat& aVideoFrame ){

    if( mUi->gBCameraStream->isChecked() ){

        QPixmap vPixmap;

        QPen LinePenRed(Qt::red,1), LinePenGreen(Qt::green,1), LinePenBlue(Qt::blue,1), LinePenYellow(Qt::yellow,1);

        QVector<QRgb> vColorTable;
        for (int i=0; i<256; i++){
            vColorTable.push_back(qRgb(i,i,i));
        }

        cv::Mat undistorted;
        if( mUi->cBUndistort->isChecked() && mDetector->getCameraFile()->isValid() ){
            cv::undistort(aVideoFrame, undistorted, mDetector->getCameraFile()->CameraMatrix, mDetector->getCameraFile()->Distorsion );
        } else {
            undistorted = aVideoFrame;
        }

        QImage img((const uchar*)undistorted.data, undistorted.cols, undistorted.rows, undistorted.step, QImage::Format_Indexed8);
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
        if( mUi->cBBorders->isChecked() ){

            QPainter painter( &vPixmap );
            if( mUi->cBUpperBorder->isChecked() ){
                painter.setPen( LinePenYellow );
            } else {
                painter.setPen( LinePenBlue );
            }
            painter.drawLine(0, mUi->lEUpperBorder->text().toInt() ,vPixmap.width(), mUi->lEUpperBorder->text().toInt() );

            if( mUi->cBLowerBorder->isChecked() ){
                painter.setPen( LinePenYellow );
            } else {
                painter.setPen( LinePenBlue );
            }
            painter.drawLine(0, mUi->lELowerBorder->text().toInt() ,vPixmap.width(), mUi->lELowerBorder->text().toInt() );

            if( mUi->cBLeftBorder->isChecked() ){
                painter.setPen( LinePenYellow );
            } else {
                painter.setPen( LinePenBlue );
            }
            painter.drawLine(mUi->lELeftBorder->text().toInt() , 0,mUi->lELeftBorder->text().toInt() , vPixmap.height() );

            if( mUi->cBRightBorder->isChecked() ){
                painter.setPen( LinePenYellow );
            } else {
                painter.setPen( LinePenBlue );
            }
            painter.drawLine(mUi->lERightBorder->text().toInt() , 0,mUi->lERightBorder->text().toInt() , vPixmap.height() );

            painter.end();

        }

        emit newPicture(vPixmap);

    }

}

void CameraGui::paintPicture(const QPixmap &aPicture){

    mUi->lblVideoStream->setPixmap(aPicture);

}

void CameraGui::pushUp( bool aDummy ){

    if( mUi->cBUpperBorder->isChecked() ){
        mUi->lEUpperBorder->setText( QString::number( mUi->lEUpperBorder->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBLowerBorder->isChecked() ){
        mUi->lELowerBorder->setText( QString::number( mUi->lELowerBorder->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBULOffset->isChecked() ){
        mUi->lEULOffset->setText( QString::number( mUi->lEULOffset->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBLROffset->isChecked() ){
        mUi->lELROffset->setText( QString::number( mUi->lELROffset->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }

    setBorderData();

}

void CameraGui::pushDown( bool aDummy ){

    if( mUi->cBUpperBorder->isChecked() ){
        mUi->lEUpperBorder->setText( QString::number( mUi->lEUpperBorder->text().toInt() - mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBLowerBorder->isChecked() ){
        mUi->lELowerBorder->setText( QString::number( mUi->lELowerBorder->text().toInt() - mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBULOffset->isChecked() ){
        mUi->lEULOffset->setText( QString::number( mUi->lEULOffset->text().toInt() - mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBLROffset->isChecked() ){
        mUi->lELROffset->setText( QString::number( mUi->lELROffset->text().toInt() -  mUi->lEPointsperClick->text().toInt() ) );
    }

    setBorderData();

}

void CameraGui::pushLeft( bool aDummy ){

    if( mUi->cBLeftBorder->isChecked() ){
        mUi->lELeftBorder->setText( QString::number( mUi->lELeftBorder->text().toInt() - mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBRightBorder->isChecked() ){
        mUi->lERightBorder->setText( QString::number( mUi->lERightBorder->text().toInt() - mUi->lEPointsperClick->text().toInt() ) );
    }

    setBorderData();

}

void CameraGui::pushRight( bool aDummy ){

    if( mUi->cBLeftBorder->isChecked() ){
        mUi->lELeftBorder->setText( QString::number( mUi->lELeftBorder->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }
    if( mUi->cBRightBorder->isChecked() ){
        mUi->lERightBorder->setText( QString::number( mUi->lERightBorder->text().toInt() + mUi->lEPointsperClick->text().toInt() ) );
    }

    setBorderData();

}

void CameraGui::setBorderData(){

    mCamera->mLowerY = mUi->lELeftBorder->text().toInt();
    mCamera->mUpperY = mUi->lERightBorder->text().toInt();
    mCamera->mLowerX = mUi->lEUpperBorder->text().toInt();
    mCamera->mUpperX = mUi->lELowerBorder->text().toInt();
    mCamera->mULMultiplier = mUi->lEULOffset->text().toInt();
    mCamera->mLRMultiplier = mUi->lELROffset->text().toInt();

}

}

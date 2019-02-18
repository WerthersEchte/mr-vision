#include "cameragui.h"
#include "ui_camera.h"

#include "src/core/detectorneural.h"
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
#include <QMessageLogger>

// #include <aruco/aruco.h>

#include <iostream>
#include <limits>


namespace mrvision {

CameraGui::CameraGui( Camera *aCamera, QWidget *aParent) :
    QWidget(aParent),
    mUi(new Ui::Camera),
    mCamera(aCamera),
    mDetector(new DetectorNeural()),
    mKnownMarkers(),
    mActiveInterface(true),
    mTakeScreenshot(false)
{
    mUi->setupUi(this);

    mUi->lblName->setText( QString::fromStdString( mCamera->getId() ) );
    mUi->lEScreenshotName->setText( QString::fromStdString(mCamera->getId()) + "_" );
    connect( mUi->pBActivateControls, SIGNAL( clicked( bool ) ), this, SLOT( showInterface( bool ) ) );

    float vMakerSize = 1.0;
    //mDetector->getParameter( MARKERSIZE, &vMakerSize );
    mUi->lEMarkerSize->setText( QString::number( vMakerSize ) );

    mUi->wCont2->hide();
    mUi->wCont3->hide();
    mUi->wCont6->hide();

    QValidator *vValidator = new QIntValidator(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), this);

    mUi->lELeftBorder->setValidator( vValidator );
    mUi->lERightBorder->setValidator( vValidator );
    mUi->lEUpperBorder->setValidator( vValidator );
    mUi->lELowerBorder->setValidator( vValidator );
    mUi->lEDALeftBorder->setValidator( vValidator );
    mUi->lEDARightBorder->setValidator( vValidator );
    mUi->lEDAUpperBorder->setValidator( vValidator );
    mUi->lEDALowerBorder->setValidator( vValidator );
    mUi->lEULOffset->setValidator( vValidator );
    mUi->lELROffset->setValidator( vValidator );

    mUi->lEScreenshotNumber->setValidator( vValidator );

    vValidator = new QDoubleValidator( 0.0, 100.0, 5, this);
    mUi->lEMarkerSize->setValidator( vValidator );


    mUi->hSThreshold->setValue( mDetector->getThreshold() );
    mUi->lEThreshold->setText( QString::number( mDetector->getThreshold() ) );
    connect( mUi->hSThreshold, SIGNAL( valueChanged( int ) ), this, SLOT( setlEThreshold( int ) ) );
    connect( mUi->lEThreshold, SIGNAL( editingFinished( ) ), this, SLOT( setThreshold( ) ) );

    unsigned int vMin, vMax;

    mCamera->getShutterMinMax( &vMin, &vMax );
    mUi->hSShutter->setMinimum( vMin );
    mUi->hSShutter->setMaximum( vMax );
    mUi->hSShutter->setValue( mCamera->getShutter() );
    mUi->hSShutter->setTracking( false );
    mUi->lEShutter->setValidator( new QIntValidator( vMin, vMax, this) );
    mUi->lEShutter->setText( QString::number(mCamera->getShutter()) );
    connect( mUi->gBShutter, SIGNAL( clicked( bool ) ), mCamera, SLOT( setShutterMode( bool ) ) );
    connect( mUi->hSShutter, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setShutter( int ) ) );
    connect( mUi->hSShutter, SIGNAL( valueChanged( int ) ), this, SLOT( setlEShutter( int ) ) );
    connect( mUi->lEShutter, SIGNAL( editingFinished( ) ), this, SLOT( setShutter( ) ) );

    mCamera->getGainMinMax( &vMin, &vMax );
    mUi->hSGain->setMinimum( vMin );
    mUi->hSGain->setMaximum( vMax );
    mUi->hSGain->setValue( mCamera->getGain() );
    mUi->hSGain->setTracking( false );
    mUi->lEGain->setValidator( new QIntValidator( vMin, vMax, this) );
    mUi->lEGain->setText( QString::number(mCamera->getGain()) );
    connect( mUi->gBGain, SIGNAL( clicked( bool ) ), mCamera, SLOT( setGainMode( bool ) ) );
    connect( mUi->hSGain, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setGain( int ) ) );
    connect( mUi->hSGain, SIGNAL( valueChanged( int ) ), this, SLOT( setlEGain( int ) ) );
    connect( mUi->lEGain, SIGNAL( editingFinished( ) ), this, SLOT( setGain( ) ) );

    mCamera->getGammaMinMax( &vMin, &vMax );
    mUi->hSGamma->setMinimum( vMin );
    mUi->hSGamma->setMaximum( vMax );
    mUi->hSGamma->setValue( mCamera->getGamma() );
    mUi->hSGamma->setTracking( false );
    mUi->lEGamma->setValidator( new QIntValidator( vMin, vMax, this) );
    mUi->lEGamma->setText( QString::number(mCamera->getGamma()) );
    connect( mUi->gBGamma, SIGNAL( clicked( bool ) ), mCamera, SLOT( setGammaMode( bool ) ) );
    connect( mUi->hSGamma, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setGamma( int ) ) );
    connect( mUi->hSGamma, SIGNAL( valueChanged( int ) ), this, SLOT( setlEGamma( int ) ) );
    connect( mUi->lEGamma, SIGNAL( editingFinished( ) ), this, SLOT( setGamma( ) ) );

    mCamera->getSharpnessMinMax( &vMin, &vMax );
    mUi->hSSharpness->setMinimum( vMin );
    mUi->hSSharpness->setMaximum( vMax );
    mUi->hSSharpness->setValue( mCamera->getSharpness() );
    mUi->hSSharpness->setTracking( false );
    mUi->lESharpness->setValidator( new QIntValidator( vMin, vMax, this) );
    mUi->lESharpness->setText( QString::number(mCamera->getSharpness()) );
    connect( mUi->gBSharpness, SIGNAL( clicked( bool ) ), mCamera, SLOT( setSharpnessMode( bool ) ) );
    connect( mUi->hSSharpness, SIGNAL( valueChanged( int ) ), mCamera, SLOT( setSharpness( int ) ) );
    connect( mUi->hSSharpness, SIGNAL( valueChanged( int ) ), this, SLOT( setlESharpness( int ) ) );
    connect( mUi->lESharpness, SIGNAL( editingFinished( ) ), this, SLOT( setSharpness( ) ) );

    connect( mUi->cBActive, SIGNAL( clicked( bool ) ), this, SLOT( detectBots( bool ) ) );
    connect( mUi->gBMarker, SIGNAL( clicked( bool ) ), this, SLOT( showMarker( bool ) ) );
    connect( mUi->gBDetection, SIGNAL( clicked( bool ) ), this, SLOT( showDetection( bool ) ) );
    connect( mUi->gBCameraData, SIGNAL( clicked( bool ) ), this, SLOT( showCameraData( bool ) ) );
    connect( mUi->gBCameraStream, SIGNAL( clicked( bool ) ), this, SLOT( startStreamingVideo( bool ) ) );
    connect( this, SIGNAL( newPicture( QPixmap ) ), this, SLOT( paintPicture( QPixmap ) ) );

    qRegisterMetaType< std::vector<DetectedMarker> >("std::vector<DetectedMarker>");
    connect( mDetector, SIGNAL( markersDetected( std::vector<DetectedMarker> ) ), mCamera, SLOT( decodeBotPositions( std::vector<DetectedMarker> ) ) );
    qRegisterMetaType< QList<mrvision::Bot> >("QList<mrvision::Bot>");
    connect( mCamera, SIGNAL( decodedBotPositions( QList<mrvision::Bot> ) ), UDPServer::getInstance(), SLOT( send_Data( QList<mrvision::Bot> ) ) );

    connect( mUi->lEMarkerSize, SIGNAL( editingFinished( ) ), this, SLOT( setMarkerSize( ) ) );

    connect( mUi->lECameraFile, SIGNAL( editingFinished( ) ), this, SLOT( setCameraFile( ) ) );
    connect( mUi->pBLoadCameraConfigFile, SIGNAL( clicked( bool ) ), this, SLOT( selectCameraFile( bool ) ) );

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
    connect(mUi->pBLoadCameraConfig, SIGNAL( clicked( bool ) ), this, SLOT( loadCameraGuiConfig( bool ) ) );

    connect( mUi->gBMarkerDetectedPictures, SIGNAL( clicked( bool ) ), this, SLOT( startStreamingMarker( bool ) ) );

    connect( mUi->pBScreenshot, SIGNAL( clicked( bool ) ), this, SLOT( takeScreenshot( bool ) ) );

    vLayout = new FlowLayout();
    vLayout->addWidget( mUi->gBShutter );
    vLayout->addWidget( mUi->gBGain );
    vLayout->addWidget( mUi->gBGamma );
    vLayout->addWidget( mUi->gBSharpness );
    mUi->gBFeatures->setLayout( vLayout );

    //TODO: dynamic
    mUi->lEDARightBorder->setText( QString::number(800) );
    mUi->lEDALowerBorder->setText( QString::number(600) );

    connect( mUi->cBShowMarker, SIGNAL( clicked( bool ) ), this, SLOT( connectLiveMarker( bool ) ) );

}

CameraGui::~CameraGui() {

    foreach( MarkerGui* vMarkerGui, mKnownMarkers ){

        delete vMarkerGui;

    }

    delete mUi;
    delete mCamera;
    delete mDetector;

}

void CameraGui::connectLiveMarker( bool aChecked ){

    if(aChecked){

        connect( mDetector, SIGNAL( markersDetected( std::vector<DetectedMarker> ) ), this, SLOT( saveCurrentMarkers( std::vector<DetectedMarker> ) ) );

    } else {

        disconnect( mDetector, SIGNAL( markersDetected( std::vector<DetectedMarker> ) ), this, SLOT( saveCurrentMarkers( std::vector<DetectedMarker> ) ) );

    }

}


void CameraGui::saveCurrentMarkers( const std::vector<DetectedMarker>& aListOfMarkers ){

    mListOfMarkers = std::vector<DetectedMarker>( aListOfMarkers );

}

void CameraGui::saveCameraGuiConfig( bool aDummy )
{
    QFile vFile( QFileDialog::getSaveFileName(this, tr("Select CameraGuiConfig"), "", tr("CameraGuiConfig Files (*.json);;All Files (*.*)")));

    if ( !vFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject vCameraConfigData, vDetectionConfigData, vBorders, vFeatures;

    vDetectionConfigData["CameraConfigFile"] = mUi->lECameraFile->text();
    vDetectionConfigData["MarkerSize"] = mUi->lEMarkerSize->text();
    vDetectionConfigData["Threshold"] = mUi->lEThreshold->text();

    vBorders["ULBorderUpper"] = mUi->lEUpperBorder->text();
    vBorders["ULBorderLower"] = mUi->lELowerBorder->text();
    vBorders["ULBorderOffset"] = mUi->lEULOffset->text();
    vBorders["LRBorderLeft"] = mUi->lELeftBorder->text();
    vBorders["LRBorderRight"] = mUi->lERightBorder->text();
    vBorders["LRBorderOffset"] = mUi->lELROffset->text();
    vBorders["DetectionAreaUpperBorder"] = mUi->lEDAUpperBorder->text();
    vBorders["DetectionAreaLowerBorder"] = mUi->lEDALowerBorder->text();
    vBorders["DetectionAreaRightBorder"] = mUi->lEDARightBorder->text();
    vBorders["DetectionAreaLeftBorder"] = mUi->lEDALeftBorder->text();

    vFeatures["Shutter"] = mUi->gBShutter->isChecked();
    vFeatures["ShutterValue"] = mUi->lEShutter->text();
    vFeatures["Gain"] = mUi->gBGain->isChecked();
    vFeatures["GainValue"] = mUi->lEGain->text();
    vFeatures["Gamma"] = mUi->gBGamma->isChecked();
    vFeatures["GammaValue"] = mUi->lEGamma->text();
    vFeatures["Sharpness"] = mUi->gBSharpness->isChecked();
    vFeatures["SharpnessValue"] = mUi->lESharpness->text();

    vCameraConfigData["CameraGUID"] = QString::fromStdString(mCamera->getId());
    vCameraConfigData["Detection"] = vDetectionConfigData;
    vCameraConfigData["Borders"] = vBorders;
    vCameraConfigData["Features"] = vFeatures;

    vCameraConfigData["ScreenshotNumber"] = mUi->lEScreenshotNumber->text();

    QJsonDocument vJsonDoc(vCameraConfigData);
    vFile.write(vJsonDoc.toJson());

}

void CameraGui::loadCameraGuiConfig( bool aDummy )
{

    QFile vFile( QFileDialog::getOpenFileName(this, tr("Select CameraGuiConfig"), "", tr("CameraGuiConfig Files (*.json);;All Files (*.*)")));

    if (!vFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray saveData = vFile.readAll();

    QJsonDocument vSavedCamera( QJsonDocument::fromJson(saveData) );
    QJsonObject vCameraConfigData( vSavedCamera.object() );

    if( vCameraConfigData["CameraGUID"].toString().compare( QString::fromStdString(mCamera->getId()) ) ){

        std::cout << "Not the same camera! (" << vCameraConfigData["CameraGUID"].toString().toStdString() << " != " << mCamera->getId() << ")" << std::endl;

    }

    QJsonObject vDetectionConfigData( vCameraConfigData["Detection"].toObject() ), vBorders( vCameraConfigData["Borders"].toObject() ), vFeatures( vCameraConfigData["Features"].toObject() );

    mUi->lEScreenshotNumber->setText( vCameraConfigData["ScreenshotNumber"].toString() );

    mUi->lECameraFile->setText( vDetectionConfigData["CameraConfigFile"].toString() );
    mUi->lEMarkerSize->setText( vDetectionConfigData["MarkerSize"].toString() );
    mUi->lEThreshold->setText( vDetectionConfigData["Threshold"].toString() );

    setCameraFile();
    setThreshold();
    setMarkerSize();

    mUi->lEUpperBorder->setText( vBorders["ULBorderUpper"].toString() );
    mUi->lELowerBorder->setText( vBorders["ULBorderLower"].toString() );
    mUi->lEULOffset->setText( vBorders["ULBorderOffset"].toString() );
    mUi->lELeftBorder->setText( vBorders["LRBorderLeft"].toString() );
    mUi->lERightBorder->setText( vBorders["LRBorderRight"].toString() );
    mUi->lELROffset->setText( vBorders["LRBorderOffset"].toString() );
    mUi->lEDAUpperBorder->setText( vBorders["DetectionAreaUpperBorder"].toString() );
    mUi->lEDALowerBorder->setText( vBorders["DetectionAreaLowerBorder"].toString() );
    mUi->lEDARightBorder->setText( vBorders["DetectionAreaRightBorder"].toString() );
    mUi->lEDALeftBorder->setText( vBorders["DetectionAreaLeftBorder"].toString() );
    setBorderData();

    mUi->gBShutter->setChecked( vFeatures["Shutter"].toBool() );
    mCamera->setShutterMode( vFeatures["Shutter"].toBool() );
    mUi->lEShutter->setText( vFeatures["ShutterValue"].toString() );
    setShutter();

    mUi->gBGain->setChecked( vFeatures["Gain"].toBool() );
    mCamera->setGainMode( vFeatures["Gain"].toBool() );
    mUi->lEGain->setText( vFeatures["GainValue"].toString() );
    setGain();

    mUi->gBGamma->setChecked( vFeatures["Gamma"].toBool() );
    mCamera->setGammaMode( vFeatures["Gamma"].toBool() );
    mUi->lEGamma->setText( vFeatures["GammaValue"].toString() );
    setGamma();

    mUi->gBSharpness->setChecked( vFeatures["Sharpness"].toBool() );
    mCamera->setSharpnessMode( vFeatures["Sharpness"].toBool() );
    mUi->lESharpness->setText( vFeatures["SharpnessValue"].toString() );
    setSharpness();

    return;

}

void CameraGui::setlEThreshold( int aValue ){

    mUi->lEThreshold->setText( QString::number( aValue ) );
    mDetector->setThreshold( mUi->lEThreshold->text().toInt() );

}

void CameraGui::setThreshold( ){

    mDetector->setThreshold( mUi->lEThreshold->text().toInt() );
    mUi->hSThreshold->setValue( mUi->lEThreshold->text().toInt() );

}

void CameraGui::setlEShutter( int aValue ){

    mUi->lEShutter->setText( QString::number( aValue ) );

}

void CameraGui::setShutter( ){

    mCamera->setShutter( mUi->lEShutter->text().toInt() );
    mUi->hSShutter->setValue( mUi->lEShutter->text().toInt() );

}

void CameraGui::setlEGain( int aValue ){

    mUi->lEGain->setText( QString::number( aValue ) );

}

void CameraGui::setGain( ){

    mCamera->setGain( mUi->lEGain->text().toInt() );
    mUi->hSGain->setValue( mUi->lEGain->text().toInt() );

}

void CameraGui::setlEGamma( int aValue ){

    mUi->lEGamma->setText( QString::number( aValue ) );

}

void CameraGui::setGamma( ){

    mCamera->setGamma( mUi->lEGamma->text().toInt() );
    mUi->hSGamma->setValue( mUi->lEGamma->text().toInt() );

}

void CameraGui::setlESharpness( int aValue ){

    mUi->lESharpness->setText( QString::number( aValue ) );

}

void CameraGui::setSharpness( ){

    mCamera->setSharpness( mUi->lESharpness->text().toInt() );
    mUi->hSSharpness->setValue( mUi->lESharpness->text().toInt() );

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
        mUi->wCont4->show();
        mUi->gBMarkerDetectedPictures->show();

    } else {

        mUi->wCont->hide();
        mUi->wCont4->hide();
        mUi->gBMarkerDetectedPictures->hide();

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

    float vMarkerSize = mUi->lEMarkerSize->text().toFloat();
    //mDetector->setParameter( MARKERSIZE, &vMarkerSize );

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
        mDetector->setStatus( true );

        if( !mCamera->isRunning() ){

            mCamera->startVideoCapture();
        }

    } else {

        disconnect( mCamera, &Camera::newVideoFrame, mDetector, &Detector::detectMarkers );
        mDetector->setStatus( false );

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
        mUi->wCont6->show();

    } else {

        disconnect( mCamera, &Camera::newVideoFrame, this, &CameraGui::streamVideo );

        mUi->wCont2->hide();
        mUi->wCont3->hide();
        mUi->wCont6->hide();

    }

}

void CameraGui::startStreamingMarker( bool aStreaming ){

    if( aStreaming ){

        qRegisterMetaType< cv::Mat >("cv::Mat");

        connect( mDetector, SIGNAL(showPotentialMarker( cv::Mat, int ) ), this, SLOT( showPotentialMarkers( cv::Mat, int ) ) );

    } else {

        disconnect( mDetector, &Detector::showPotentialMarker, this, &CameraGui::showPotentialMarkers );

    }

}


void CameraGui::showPotentialMarkers( const cv::Mat& aImage, const int& aIteration){
	
	if (mCurrentMarkerIteration != aIteration ) {
		mCurrentMarkerIteration = aIteration;
		delete mUi->gBMarkerDetectedPictures->layout();

		FlowLayout *vLayout = new FlowLayout();
		mUi->gBMarkerDetectedPictures->setLayout(vLayout);

	}
	QImage vPotentialMarkerImage((const uchar*)aImage.data, aImage.cols, aImage.rows, aImage.step, QImage::Format_Indexed8);
	QLabel* vPotentialMarker = new QLabel();
	vPotentialMarker->setPixmap(QPixmap::fromImage(vPotentialMarkerImage));
	
	mUi->gBMarkerDetectedPictures->layout()->addWidget(vPotentialMarker);

}

void CameraGui::streamVideo( const cv::Mat& aVideoFrame ){

    QtConcurrent::run(this, &CameraGui::createPictureFromVideoframe, aVideoFrame);

}

void CameraGui::createPictureFromVideoframe( const cv::Mat& aVideoFrame ){

    if( mUi->gBCameraStream->isChecked() ){

        QPixmap vPixmap;

        QPen LinePenRed(Qt::red,1), LinePenRedFat(Qt::red,5), LinePenGreen(Qt::green,1), LinePenBlue(Qt::blue,1), LinePenYellow(Qt::yellow,1), LinePenWhite(Qt::white,1);

        QVector<QRgb> vColorTable;
        for (int i=0; i<256; i++){
            vColorTable.push_back(qRgb(i,i,i));
        }

        cv::Mat undistorted, sharpie;
        if( mUi->cBUndistort->isChecked() ){//&& mDetector->getCameraFile()->isValid() ){
            cv::undistort(aVideoFrame, undistorted, mDetector->mCameraMatrix, mDetector->mDistortionMatrix );
        cv::GaussianBlur(undistorted, sharpie, cv::Size(0, 0), 3);
        cv::addWeighted(sharpie, -0.5, undistorted, 1.5, 0, undistorted);
        } else {
            undistorted = aVideoFrame;
        }

        QImage img((const uchar*)undistorted.data, undistorted.cols, undistorted.rows, undistorted.step, QImage::Format_Indexed8);
        img.setColorTable(vColorTable);

        vPixmap = QPixmap::fromImage( img );

        if( mTakeScreenshot ){
            mTakeScreenshot = false;
            vPixmap.save( QString( mUi->lEScreenshotName->text() + mUi->lEScreenshotNumber->text() + mUi->lEScreenshotEnding->text() ) );
            mUi->lEScreenshotNumber->setText( QString("%1").arg( mUi->lEScreenshotNumber->text().toInt() + 1, 5, 10, QChar('0') ) );

        }

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
            painter.setPen( LinePenYellow );
            painter.drawLine(0, mUi->lEUpperBorder->text().toInt() ,vPixmap.width(), mUi->lEUpperBorder->text().toInt() );
            painter.drawLine(0, mUi->lELowerBorder->text().toInt() ,vPixmap.width(), mUi->lELowerBorder->text().toInt() );
            painter.drawLine(mUi->lELeftBorder->text().toInt() , 0,mUi->lELeftBorder->text().toInt() , vPixmap.height() );
            painter.drawLine(mUi->lERightBorder->text().toInt() , 0,mUi->lERightBorder->text().toInt() , vPixmap.height() );
            painter.setPen( LinePenWhite );
            painter.fillRect(0, 0,vPixmap.width(), mUi->lEDAUpperBorder->text().toInt(), Qt::SolidPattern );
            painter.fillRect(0, 0, mUi->lEDALeftBorder->text().toInt(), vPixmap.height(), Qt::SolidPattern );
            painter.fillRect(mUi->lEDARightBorder->text().toInt() , 0, vPixmap.width(), vPixmap.height(), Qt::SolidPattern );
            painter.fillRect(0, mUi->lEDALowerBorder->text().toInt() , vPixmap.width(), vPixmap.height(), Qt::SolidPattern );

            painter.end();

        }
        if( mUi->cBShowMarker->isChecked() ){
            std::vector<DetectedMarker> vListOfMarkers(mListOfMarkers);
            QPainter painter( &vPixmap );
            for( DetectedMarker vMarker : vListOfMarkers ){

                QLineF angleline;
                angleline.setP1(QPointF(vMarker.mMarkerPosition.x,vMarker.mMarkerPosition.y));

                angleline.setAngle(vMarker.mMarkerDirection*-1);
                angleline.setLength(5);

                painter.setPen( LinePenRedFat );
                painter.drawLine(angleline);

                angleline.setLength(15);
                painter.setPen( LinePenRed );
                painter.drawLine(angleline);
                painter.setPen( LinePenYellow );
                painter.drawText(vMarker.mMarkerPosition.x,vMarker.mMarkerPosition.y, QString::number(vMarker.mMarkerId));

            }

            painter.end();

        }

        emit newPicture(vPixmap);

    }

}

void CameraGui::paintPicture(const QPixmap &aPicture){

    mUi->lblVideoStream->setPixmap(aPicture);

}

void CameraGui::takeScreenshot( bool aScreenshot ){

    mTakeScreenshot = true;

}

void CameraGui::setBorderData(){

    mCamera->mLowerX = mUi->lELeftBorder->text().toInt();
    mCamera->mUpperX = mUi->lERightBorder->text().toInt();
    mCamera->mUpperY = mUi->lEUpperBorder->text().toInt();
    mCamera->mLowerY = mUi->lELowerBorder->text().toInt();
    mCamera->mULMultiplier = mUi->lEULOffset->text().toInt();
    mCamera->mLRMultiplier = mUi->lELROffset->text().toInt();

    mDetector->setDetectionArea( mUi->lEDALeftBorder->text().toInt(), mUi->lEDARightBorder->text().toInt(), mUi->lEDAUpperBorder->text().toInt(), mUi->lEDALowerBorder->text().toInt() );

}

}

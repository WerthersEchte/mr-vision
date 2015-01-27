#include "markergui.h"
#include "ui_marker.h"

#include <iostream>

#include <QTime>

namespace mrvision {

MarkerGui::MarkerGui( const Marker& aMarker, QWidget *aParent ) :
    QWidget(aParent),
    mUi(new Ui::Marker),
    mIsBlinking(false),
    mBorderBlack(),
    mBorderRed()
{

    mUi->setupUi(this);
    this->setMaximumWidth(76);
    mUi->fMarker->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    mMarkerId = aMarker.getId();
    std::vector<bool> vMaker = aMarker.getMakerRotation();

    mBorderRed.setColor(QPalette::Light, QColor(Qt::red));
    mBorderRed.setColor(QPalette::Dark, QColor(Qt::red));

    QPalette vPaletteBlack = mUi->lbl1->palette(), vPaletteWhite = mUi->lbl1->palette();
    vPaletteBlack.setColor(QPalette::Window, QColor(Qt::black));
    vPaletteBlack.setColor(QPalette::WindowText, QColor(Qt::white));
    vPaletteWhite.setColor(QPalette::Window, QColor(Qt::white));
    vPaletteWhite.setColor(QPalette::WindowText, QColor(Qt::black));

    mUi->lbl5->setText( QString::number( mMarkerId ) );

    mUi->lbl1->setPalette( vMaker.at(0) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl2->setPalette( vMaker.at(1) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl3->setPalette( vMaker.at(2) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl4->setPalette( vMaker.at(3) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl5->setPalette( vMaker.at(4) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl6->setPalette( vMaker.at(5) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl7->setPalette( vMaker.at(6) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl8->setPalette( vMaker.at(7) ? vPaletteWhite : vPaletteBlack );
    mUi->lbl9->setPalette( vMaker.at(8) ? vPaletteWhite : vPaletteBlack );



}

MarkerGui::~MarkerGui() {

    delete mUi;

}

int MarkerGui::getId() {

    return mMarkerId;

}


void MarkerGui::blink( int aId ){

    if( aId == mMarkerId && !mIsBlinking ){

        mIsBlinking = true;
        mUi->fMarker->setFrameStyle( QFrame::Panel | QFrame::Raised );
        mUi->fMarker->setPalette(mBorderRed);
        delay();
        mUi->fMarker->setFrameStyle( QFrame::Panel | QFrame::Sunken );
        mUi->fMarker->setPalette(mBorderBlack);
        mIsBlinking = false;

    }

}

void MarkerGui::delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(500);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

}

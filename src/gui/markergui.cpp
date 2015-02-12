#include "markergui.h"
#include "ui_marker.h"

#include <iostream>

#include <QTime>
#include <QPainter>
#include <QColor>

namespace mrvision {

MarkerGui::MarkerGui( const Marker& aMarker, QWidget *aParent ) :
    QWidget(aParent),
    mUi(new Ui::Marker),
    mIsBlinking(false),
    mBorderBlack(),
    mBorderRed(),
    mBlack(QColor(Qt::black)),
    mWhite(QColor(Qt::white)),
    mMarker(aMarker)
{

    mUi->setupUi(this);
    this->setMaximumWidth(76);
    mUi->fMarker->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    mBorderRed.setColor(QPalette::Light, QColor(Qt::red));
    mBorderRed.setColor(QPalette::Dark, QColor(Qt::red));

    //mUi->lbl5->setText( QString::number( mMarkerId ) );

    repaint();
}

MarkerGui::~MarkerGui() {

    delete mUi;

}

void MarkerGui::paintEvent(QPaintEvent *event)
{

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for( int vHeight = 0; vHeight < mMarker.getHeight(); ++vHeight ){

        for( int vWidth = 0; vWidth < mMarker.getWidth(); ++vWidth ){

            painter.fillRect( QRect( 4 + 66 / mMarker.getWidth() * vWidth, 4 + 66 / mMarker.getHeight() * vHeight,
                                     66 / mMarker.getHeight(), 66 / mMarker.getHeight() ),
                              mMarker.getMakerRotation().at( mMarker.getWidth() * vHeight + vWidth ) ? mWhite : mBlack);

        }

    }

    painter.setPen(QPen(Qt::red));
    painter.drawText(rect(), Qt::AlignCenter, QString::number( mMarker.getId() ) );
    painter.end();
}

int MarkerGui::getId() {

    return mMarker.getId();

}


void MarkerGui::blink( int aId ){

    if( aId == mMarker.getId() && !mIsBlinking ){

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

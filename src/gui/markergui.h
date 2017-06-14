#ifndef MARKERGUI_H
#define MARKERGUI_H

#include <QWidget>
#include <QThread>
#include <QBrush>
#include <QPaintEvent>
#include "src/core/marker.h"

namespace Ui {
	class Marker;
}

namespace mrvision{

class MarkerGui : public QWidget
{
	Q_OBJECT

	Marker mMarker;

	Ui::Marker *mUi;
	QPalette mBorderBlack, mBorderRed;
	QBrush mBlack, mWhite;

	bool mIsBlinking;

public:
    explicit MarkerGui( const Marker& aMarker, QWidget *aParent = 0 );
    ~MarkerGui();

    int getId();

public slots:
    void blink( int aId );

private:
    void delay();
protected:
    void paintEvent(QPaintEvent *event);

};

class EventThread : public QThread
{

    Q_OBJECT
public:
    void run( void )
    {
        exec();
    }

};

}

#endif // MARKERGUI_H

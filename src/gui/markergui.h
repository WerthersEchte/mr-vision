#ifndef MARKERGUI_H
#define MARKERGUI_H

#include <QWidget>
#include <QThread>
#include "src/core/marker.h"

namespace Ui {
	class Marker;
}

namespace mrvision{

class MarkerGui : public QWidget
{
	Q_OBJECT

	int mMarkerId;
	Ui::Marker *mUi;
	QPalette mBorderBlack, mBorderRed;

	bool mIsBlinking;

public:
    explicit MarkerGui( const Marker& aMarker, QWidget *aParent = 0 );
    ~MarkerGui();

    int getId();

public slots:
    void blink( int aId );

private:
    void delay();

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

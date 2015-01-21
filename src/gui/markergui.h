#ifndef MARKERGUI_H
#define MARKERGUI_H

#include <QWidget>
#include "src/core/marker.h"

namespace Ui {
	class Marker;
}

namespace mrvision{

class MarkerGui : public QWidget
{
	Q_OBJECT

	int mMakerId;
	Ui::Marker *mUi;

public:
    explicit MarkerGui( const Marker& aMarker, QWidget *aParent = 0 );
    ~MarkerGui();

};

}

#endif // MARKERGUI_H

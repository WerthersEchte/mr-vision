#ifndef NETWORKGUI_H
#define NETWORKGUI_H

#include <QWidget>

namespace Ui {
	class Network;
}

namespace mrvision{

class NetworkGui : public QWidget
{
	Q_OBJECT

	Ui::Network *mUi;

public:
    explicit NetworkGui( QWidget *aParent = 0 );
    ~NetworkGui();

private slots:
    void changedPort();
    void changedPacketime();

    void startstopServer( bool aDummy );

    void changedServerStatus( bool aStatus );

};

}

#endif // NETWORKGUI_H

#include "networkgui.h"
#include "ui_network.h"

#include "src/core/udp_server.h"

namespace mrvision {

NetworkGui::NetworkGui( QWidget *aParent ) :
    QWidget(aParent),
    mUi(new Ui::Network)
{

    mUi->setupUi(this);

    mUi->lEPort->setText(QString::number( UDPServer::getInstance()->getPort() ));
    mUi->lEPacketTime->setText(QString::number( UDPServer::getInstance()->getPacketTime() ));

    connect( mUi->lEPort, SIGNAL( editingFinished( ) ), this, SLOT( changedPort( ) ) );
    connect( mUi->lEPacketTime, SIGNAL( editingFinished( ) ), this, SLOT( changedPacketime( ) ) );
    connect( mUi->pBStartStop, SIGNAL( clicked( bool ) ), this, SLOT( startstopServer( bool ) ) );
    connect( UDPServer::getInstance(), SIGNAL( statusServer( bool ) ), this, SLOT( changedServerStatus( bool ) ) );

}

NetworkGui::~NetworkGui() {

    delete mUi;

}

void NetworkGui::changedPort(){

    UDPServer::getInstance()->setPort( mUi->lEPort->text().toInt() );

}

void NetworkGui::changedPacketime(){

    UDPServer::getInstance()->setPacketTime( mUi->lEPacketTime->text().toInt() );

}

void NetworkGui::startstopServer( bool aDummy ){

    if( UDPServer::getInstance()->startServer() ){

        mUi->pBStartStop->setText(QString("Stop"));

    } else if( UDPServer::getInstance()->stopServer() ){

        mUi->pBStartStop->setText(QString("Start"));

    }

}

void NetworkGui::changedServerStatus( bool aStatus ){

    if( aStatus ){

        mUi->pBStartStop->setText(QString("Stop"));

    } else {

        mUi->pBStartStop->setText(QString("Start"));

    }


}


}

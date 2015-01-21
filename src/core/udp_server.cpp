#include "udp_server.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <string>
#include <boost/asio.hpp>


namespace mrvision {

enum { max_length = 64000 };

UDPServer* UDPServer::getInstance(){

    if( INSTANCE == nullptr ){

        INSTANCE = new UDPServer();

    }

    return INSTANCE;

}

UDPServer::UDPServer() :
	mSocket( m_io_service ),
	hasToSendData(false),
	mDataToSend(512),
	mPort(9050),
	mPacketTime(20)
{

    mSocket.open(boost::asio::ip::udp::v4());
    mSocket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort ));

}

UDPServer::~UDPServer(){

    while( isRunning() ){

        hasToSendData = false;

    }

}

bool UDPServer::startServer()
{

    if( !isRunning() && !hasToSendData ){

        mSocket.close();
        mSocket.open(boost::asio::ip::udp::v4());
        mSocket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), mPort ));
        hasToSendData = true;
        start();
        return true;

    }

    hasToSendData = false;

    return false;

}

bool UDPServer::stopServer()
{

    if( isRunning() && hasToSendData ){

        hasToSendData = false;
        return true;

    }

    return false;

}

int UDPServer::getPort()
{

    return mPort;

}

int UDPServer::getPacketTime()
{

    return mPacketTime;

}

void UDPServer::setPort( int aPort )
{

    mPort = aPort;

}

void UDPServer::setPacketTime( int aPacketTime )
{

    mPacketTime = aPacketTime;

}

void UDPServer::send_Data( const QList<mrvision::Bot>& mBots ){

    if( isRunning() ){

        foreach( mrvision::Bot vBot, mBots ){

            mDataToSend.push(vBot);

            std::cout << vBot.getId() << "(" << vBot.getX() << "/" <<  vBot.getY() << ") " << std::endl;

        }
    }

}

std::string vStartDataPackage("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<positiondatapackage>"
		"<visionmode>VISION_MODE_STREAM_ALL</visionmode>" );
std::string vEndDataPackage( "</positiondatapackage>" );

void UDPServer::run(){

    emit statusServer(true);

	std::string vAcknowlege("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
			"<connectionacknowlege>"
			"<servername>bratwurst</servername>"
			"<clientname>bockwurst</clientname>"
			"<connectionallowed>true</connectionallowed>"
			"</connectionacknowlege>");
	char data[max_length];
	size_t length = mSocket.receive_from(
	boost::asio::buffer(data, max_length), sender_endpoint);
	mSocket.send_to(boost::asio::buffer( vAcknowlege.c_str(), vAcknowlege.length() ), sender_endpoint);

    mrvision::Bot vBot;
    bool vDataToSend;
    std::clock_t tStart;

    while( hasToSendData ){

        tStart = std::clock();

        vDataToSend = false;
        std::stringstream vPositionData;
        vPositionData << vStartDataPackage;

        for( int i = 0; i < 5 && mDataToSend.pop( vBot ); ++i ){

            vDataToSend = true;
            vPositionData << "<objects xsi:type=\"positionObjectBot\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"><objecttype>BOT</objecttype><id>";
            vPositionData << vBot.getId();
            vPositionData << "</id><name>Default</name><location>";
            vPositionData << vBot.getX();
            vPositionData << "</location><location>";
            vPositionData << vBot.getY();
            vPositionData << "</location><color>0.0</color><color>0.0</color><angle>";
            vPositionData << vBot.getAngle();
            vPositionData << "</angle></objects>";

        }

        if( vDataToSend ){
            vPositionData << vEndDataPackage;

            mSocket.send_to(boost::asio::buffer( vPositionData.str().c_str(), vPositionData.str().length() ), sender_endpoint);
        }

        while(((double)(std::clock() - tStart)/CLOCKS_PER_SEC) < (double) mPacketTime/1000 ){ }

    }

    mSocket.close();

    emit statusServer(false);

}



}

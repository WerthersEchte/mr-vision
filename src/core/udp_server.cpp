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

        INSTANCE = new UDPServer( 9050 );

    }

    return INSTANCE;

}

UDPServer::UDPServer( int aPort ) :
	mSocket( m_io_service ),
	hasToSendData(false),
	mDataToSend(512)
{

    mSocket.open(boost::asio::ip::udp::v4());
    mSocket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), aPort ));

    if (!mDataToSend.is_lock_free())
        std::cout << "not ";
    std::cout << "lockfree" << std::endl;

}

UDPServer::~UDPServer(){

    while( isRunning() ){

        hasToSendData = false;

    }

}

void UDPServer::startServer()
{
    hasToSendData = true;
    start();

}

void UDPServer::send_Data( const QList<mrvision::Bot>& mBots ){

    if( isRunning() ){

        foreach( mrvision::Bot vBot, mBots ){

            mDataToSend.push(vBot);

        }
    }

}

std::string vStartDataPackage("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<positiondatapackage>"
		"<visionmode>VISION_MODE_STREAM_ALL</visionmode>" );
std::string vEndDataPackage( "</positiondatapackage>" );

void UDPServer::run(){

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

            std::cout << vBot.getId() << "(" << vBot.getX() << "/" <<  vBot.getY() << ") ";

        }

        std::cout << std::endl;

        if( vDataToSend ){
            vPositionData << vEndDataPackage;

            mSocket.send_to(boost::asio::buffer( vPositionData.str().c_str(), vPositionData.str().length() ), sender_endpoint);
        }

        while(((double)(std::clock() - tStart)/CLOCKS_PER_SEC) < 0.02 ){ }

    }

}



}

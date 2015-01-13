#include "udp_server.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>


namespace mrvision {

enum { max_length = 64000 };

UDPServer::UDPServer( int aPort ) :
	mSocket( m_io_service )
{

    mSocket.open(boost::asio::ip::udp::v4());
    mSocket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), aPort ));

}

void UDPServer::start()
{


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

}

std::string vStartDataPackage("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<positiondatapackage>"
		"<visionmode>VISION_MODE_STREAM_ALL</visionmode>" );
std::string vEndDataPackage( "</positiondatapackage>" );

void UDPServer::send_Data( std::map<int, Bot> mBots ){
	std::stringstream vPositionData;
	vPositionData << vStartDataPackage;

	for( std::pair<const int, mrvision::Bot> vBot : mBots ){

		vPositionData << "<objects xsi:type=\"positionObjectBot\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"><objecttype>BOT</objecttype><id>";
		vPositionData << vBot.second.getId();
		vPositionData << "</id><name>Default</name><location>";
		vPositionData << vBot.second.getX();
		vPositionData << "</location><location>";
		vPositionData << vBot.second.getY();
		vPositionData << "</location><color>0.0</color><color>0.0</color><angle>";
		vPositionData << vBot.second.getAngle();
		vPositionData << "</angle></objects>";

	}

	vPositionData << vEndDataPackage;

	mSocket.send_to(boost::asio::buffer( vPositionData.str().c_str(), vPositionData.str().length() ), sender_endpoint);

}

}

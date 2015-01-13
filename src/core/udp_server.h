#ifndef _udp_server_h
#define _udp_server_h

#include <boost/asio.hpp>

#include <map>
#include "bot.h"

namespace mrvision {

class UDPServer{

	boost::asio::io_service m_io_service;
	boost::asio::ip::udp::socket mSocket;
	boost::asio::ip::udp::endpoint sender_endpoint;

public:
	UDPServer( int aPort );

	void start();
	void send_Data( std::map<int, Bot> mBots );
private:

};


}

#endif // _udp_server_h

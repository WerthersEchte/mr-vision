#ifndef _udp_server_h
#define _udp_server_h

#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>

#include <map>
#include "bot.h"

#include <QThread>
#include <QList>

namespace mrvision {

class UDPServer : public QThread{

    Q_OBJECT

	boost::asio::io_service m_io_service;
	boost::asio::ip::udp::socket mSocket;
	boost::asio::ip::udp::endpoint sender_endpoint;

	bool hasToSendData;
	boost::lockfree::queue<mrvision::Bot> mDataToSend;

	UDPServer( int aPort );

public:
    ~UDPServer();

    static UDPServer* getInstance();

	void startServer();

public slots:
	void send_Data( const QList<mrvision::Bot>& mBots );

private:
    void run();

};

static UDPServer *INSTANCE;


}

#endif // _udp_server_h

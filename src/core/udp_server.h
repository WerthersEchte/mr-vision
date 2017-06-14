#ifndef _udp_server_h
#define _udp_server_h

#define WIN32_LEAN_AND_MEAN 1
#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>
#include "tisudshl.h"

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

	int mPort, mPacketTime;

	UDPServer();

public:
    ~UDPServer();

    static UDPServer* getInstance();

	bool startServer();
	bool stopServer();

	int getPort();
	void setPort( int aPort );
	int getPacketTime();
	void setPacketTime( int aPort );

public slots:
	void send_Data( const QList<mrvision::Bot>& mBots );

signals:
    void statusServer( bool isRunning = false );

private:
    void run();

};

static UDPServer *INSTANCE;


}

#endif // _udp_server_h

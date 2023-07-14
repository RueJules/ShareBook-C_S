#ifndef BOOSTNETWORK_H
#define BOOSTNETWORK_H
#include <QJsonObject>
#include <boost/asio.hpp>
#include "control.h"
#include <memory>

using namespace boost::asio::ip;

class BoostNetwork:public std::enable_shared_from_this<BoostNetwork>
{
public:
    BoostNetwork(tcp::socket s);
    void start();

private:
    void doRead();
    void doWrite(QByteArray &result);
private:
    tcp::socket m_socket;
    enum{MAXLENGTH = 1024};
    boost::asio::streambuf dataReceived;
    boost::asio::mutable_buffer dataToSend;
    Control *_control;
};

#endif // BOOSTNETWORK_H

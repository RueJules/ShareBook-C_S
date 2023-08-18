#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <QByteArray>

using namespace boost::asio::ip;

class Socket:public std::enable_shared_from_this<Socket>
{

public:
    Socket(boost::asio::io_service& io_service, const std::string& host, const int& port);
    void do_write();
    void do_read();
    void start();

private:

    static boost::asio::io_service service;
    //boost::shared_ptr<boost::asio::io_service::work> work;

    tcp::socket m_socket;
    boost::asio::mutable_buffer write_buffer;
    char read_buffer[1024];
};

#endif // SOCKET_H

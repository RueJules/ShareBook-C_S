#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <QByteArray>

using namespace boost::asio;
class Control;
class Client: public std::enable_shared_from_this<Client>{

public:
    Client(boost::asio::io_service& io_service, const std::string& host, const int& port, Control* control_ptr);

    void do_write(QByteArray);
    void do_read();


private:
    static boost::asio::io_service service;
    boost::shared_ptr<io_service::work> work;
    boost::thread thread;

    Control *control;

    ip::tcp::socket socket;

    boost::asio::mutable_buffer write_buffer;
    boost::asio::streambuf read_buffer;
    std::mutex write_mutex;
};

#endif // CLIENT_H

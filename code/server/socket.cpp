#include "socket.h"

Socket::Socket(boost::asio::io_service &io_service, const std::string &host, const int &port):m_socket(io_service), work{new boost::asio::io_service::work(service)}
{
    tcp::endpoint ep(ip::address::from_string(host), port);
    m_socket.async_connect(ep,[this](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "连接成功！" << std::endl;
        }
        else {
            std::cerr << "连接失败：" << ec.message() << std::endl;
        }
    });
}

void Socket::do_write()
{

}

void Socket::on_write()
{

}

void Socket::do_read()
{

}

void Socket::on_read(){

}

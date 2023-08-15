#include "server.h"
#include <boost/asio.hpp>
#include<iostream>
#include <vector>
#include "socket.h"

using namespace boost::asio;

io_service Server::service_2001;
io_service Server::service_9999;

Server::Server(/*unsigned short port*/): m_acc(service_2001,ip::tcp::endpoint(ip::tcp::v4(), 2001)), work{new boost::asio::io_service::work(service)}
{
    for ( int i = 0; i < 100; ++i)
        threads.create_thread(boost::bind(&boost::asio::io_service::run, &service_2001));

}

void Server::startAccept()
{
    m_acc.async_accept([this](const boost::system::error_code &e, ip::tcp::socket s){
        if(!e){
            qDebug() << "来了！\n";
            auto boostNetwork = std::make_shared<BoostNetwork>(std::move(s));
            boostNetwork->start();

        }
       startAccept();
    });
}

//将视频信息发送给流媒体服务器
void Server::startCommunicate()
{
    Socket sock(service_9999,"", );


}

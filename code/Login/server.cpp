#include "server.h"
#include <boost/asio.hpp>
#include<iostream>
#include <vector>
using namespace boost::asio;

io_service Server::service;

Server::Server(/*unsigned short port*/): m_acc(service,ip::tcp::endpoint(ip::tcp::v4(), 2001)), work{new boost::asio::io_service::work(service)}
{
    for ( int i = 0; i < 5; ++i)
        threads.create_thread(boost::bind(&boost::asio::io_service::run, &service));
}

void Server::startAccept()
{
    m_acc.async_accept([this](const boost::system::error_code &e, ip::tcp::socket s){
        if(!e){
            std::cout << "来了！\n";
            auto boostNetwork = std::make_shared<BoostNetwork>(std::move(s));
            boostNetwork->start();
            //sockets.emplace_back(boostNetwork);

        }
       startAccept();
    });
}

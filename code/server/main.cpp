#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "server.h"
#include <boost/asio.hpp>
#include "relationalBroker.h"
#include "control.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "netizenBroker.h"
#include "noteBroker.h"
#include "materialBroker.h"
#include "commentbroker.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    RelationalBroker relationalBroker;
    relationalBroker.initDataBase();
   // relationalBroker.sycn();
    std::thread  *thread=new std::thread(&RelationalBroker::sycn,&relationalBroker);



    //服务器开始监听
    Server server;
    std::thread(&Server::startAccept, server).join();
    std::thread(&Server::startCommunicate, server).join();

    thread->join();

    return app.exec();
}

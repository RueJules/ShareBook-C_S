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


    std::shared_ptr<NetizenBroker> netizenBroker = NetizenBroker::getInstance();
    std::shared_ptr<NoteBroker> noteBroker = NoteBroker::getInstance();
    std::shared_ptr<MaterialBroker> materialsBroker = MaterialBroker::getInstance();
    std::shared_ptr<CommentBroker> commentBroker = CommentBroker::getInstance();

    //缓存初始化
    netizenBroker->initCache();
    noteBroker->initCache();
    materialsBroker->initCache();
    commentBroker->initCache();

    //启动缓存同步线程
    netizenBroker->start_thread();
    noteBroker->start_thread();
    materialsBroker->start_thread();
    commentBroker->start_thread();

    //服务器开始监听
    Server server;
    server.startAccept();

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/Login/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

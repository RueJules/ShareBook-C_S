#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "server.h"
#include <boost/asio.hpp>
#include "relationalBroker.h"
#include "control.h"
#include <QJsonObject>
#include <QJsonDocument>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //RelationalBroker *db = new RelationalBroker;
    //db->initDataBase();


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

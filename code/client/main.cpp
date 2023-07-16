#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "client.h"
#include "control.h"
#include "boost/thread.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Control *control=new Control();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("control", control);
    //engine.addImageProvider("material", control->getImageProvider());
    engine.addImageProvider("material", control->getMaterialProvider());
    engine.addImageProvider("profile", control->getProfileProvider());

    const QUrl url(u"qrc:/client/Login.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "clienthandle.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<ClientHandle>("Drone.ClientHandle", 1, 0, "ClientHandle");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

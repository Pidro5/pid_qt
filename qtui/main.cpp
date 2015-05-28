#include <QApplication>
#include <QQmlApplicationEngine>
#include "gamethread.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GameThread gameThread;
    gameThread.start();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

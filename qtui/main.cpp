#include <QApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include "gamethread.h"
#include "qplayer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // TODO: Use PIDRO_ROOT.
    QDir::setCurrent("../qtui");

    QPlayer player;
    GameThread gameThread(&player);
    gameThread.start();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

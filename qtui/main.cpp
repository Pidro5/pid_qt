#include <QApplication>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "gamethread.h"
#include "qplayer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(cards);

    // TODO: Use PIDRO_ROOT.
    QDir::setCurrent("../qtui");

    QPlayer player;
    GameThread gameThread(&player);
    gameThread.start();

    QQmlApplicationEngine engine;
    QPlayer::declareQML();
    engine.rootContext()->setContextProperty("player", &player);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

#include <QApplication>
#include <QQmlApplicationEngine>
#include "qplayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPlayer player("Victor");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

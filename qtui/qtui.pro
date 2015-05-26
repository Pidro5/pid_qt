TEMPLATE = app

CONFIG += c++11

QT += qml quick widgets

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../common

HEADERS += qplayer.h

SOURCES += ../common/bidengine.cpp
SOURCES += ../common/deck_card.cpp
SOURCES += ../common/event.cpp
SOURCES += ../common/game.cpp
SOURCES += ../common/log.cpp
SOURCES += ../common/player.cpp
SOURCES += ../common/playengine.cpp
SOURCES += ../common/playercomputer.cpp
SOURCES += ../common/rotator.cpp
SOURCES += main.cpp
SOURCES += qplayer.cpp

RESOURCES += qml.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:unix: LIBS += -L$$PWD/../lualib/ -llua51

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

TEMPLATE = app

CONFIG += c++11

QT += qml quick widgets

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../common

HEADERS += ../common/pidro/bidengine.h
HEADERS += ../common/pidro/deck_card.h
HEADERS += ../common/pidro/event.h
HEADERS += ../common/pidro/game.h
HEADERS += ../common/pidro/log.h
HEADERS += ../common/pidro/playengine.h
HEADERS += ../common/pidro/player.h
HEADERS += ../common/pidro/playercomputer.h
HEADERS += ../common/pidro/rotator.h
HEADERS += gamethread.h
HEADERS += playerproxy.h
HEADERS += qpidroevent.h
HEADERS += qpidroresult.h
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
SOURCES += gamethread.cpp
SOURCES += main.cpp
SOURCES += playerproxy.cpp
SOURCES += qpidroevent.cpp
SOURCES += qpidroresult.cpp
SOURCES += qplayer.cpp

RESOURCES += qml.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:unix: LIBS += -L$$PWD/../lualib/ -llua51

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

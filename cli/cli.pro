#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T11:19:27
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Build_002
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app

INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD/../common
INCLUDEPATH += $$PWD/../lualib
               
DEPENDPATH += $$INCLUDEPATH

HEADERS += playerconsole.h
HEADERS += ../common/pidro/playengine.h
HEADERS += ../common/pidro/deck_card.h
HEADERS += ../common/pidro/event.h
HEADERS += ../common/pidro/game.h
HEADERS += ../common/pidro/log.h
HEADERS += ../common/pidro/player.h
HEADERS += ../common/pidro/playercomputer.h
HEADERS += ../common/pidro/rotator.h
HEADERS += ../common/pidro/bidengine.h
HEADERS += ../luah/lauxlib.h
HEADERS += ../luah/lua.h
HEADERS += ../luah/lua.hpp
HEADERS += ../luah/luaconf.h
HEADERS += ../luah/lualib.h

SOURCES += main.cpp
SOURCES += playerconsole.cpp
SOURCES += ../common/playengine.cpp
SOURCES += ../common/deck_card.cpp
SOURCES += ../common/event.cpp
SOURCES += ../common/game.cpp
SOURCES += ../common/log.cpp
SOURCES += ../common/player.cpp
SOURCES += ../common/playercomputer.cpp
SOURCES += ../common/rotator.cpp
SOURCES += ../common/bidengine.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lualib/ -llua51
else:unix: LIBS += -L$$PWD/../lualib/ -llua51

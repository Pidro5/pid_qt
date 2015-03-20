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


SOURCES += main.cpp \
     cli/playerconsole.cpp \
    common/deck_card.cpp \
    common/game.cpp \
    common/log.cpp \
    common/player.cpp \
    common/playercomputer.cpp \
    common/rotator.cpp \
    common/bidengine.cpp

HEADERS += \
    cli/playerconsole.h \
    common/deck_card.h \
    common/event.h \
    common/game.h \
    common/log.h \
    common/player.h \
    common/playercomputer.h \
    common/rotator.h \
    common/bidengine.h \
    luah/lauxlib.h \
    luah/lua.h \
    luah/lua.hpp \
    luah/luaconf.h \
    luah/lualib.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lualib/ -llua51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lualib/ -llua51
else:unix: LIBS += -L$$PWD/lualib/ -llua51

INCLUDEPATH += $$PWD/lualib
DEPENDPATH += $$PWD/lualib

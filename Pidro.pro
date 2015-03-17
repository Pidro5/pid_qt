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
    common/ea.cpp \
    common/game.cpp \
    common/log.cpp \
    common/player.cpp \
    common/playercomputer.cpp \
    common/rotator.cpp

HEADERS += \
    cli/playerconsole.h \
    common/deck_card.h \
    common/ea.h \
    common/event.h \
    common/game.h \
    common/log.h \
    common/player.h \
    common/playercomputer.h \
    common/rotator.h

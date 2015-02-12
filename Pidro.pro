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

TEMPLATE = app


SOURCES += main.cpp \
    deck_card.cpp \
    log.cpp \
    game.cpp \
    player.cpp \
    rotator.cpp \
    ea/ea.cpp \
    playerconsole.cpp

HEADERS += \
    deck_card.h \
    log.h \
    game.h \
    player.h \
    rotator.h \
    ea/ea.h \
    event.h \
    playerconsole.h

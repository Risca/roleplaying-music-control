#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T15:32:14
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = roleplaying-music-control
TEMPLATE = app

LIBS += -lspotify
QMAKE_CFLAGS += -Wno-unused-parameter

SOURCES += main.cpp\
        mainwindow.cpp \
    spotify.cpp \
    appkey.c \
    spotify_ll.c \
    spotifyaudioworker.cpp

HEADERS  += mainwindow.h \
    spotify.h \
    threadsafequeue.h \
    spotify_ll.h \
    spotifyaudioworker.h

FORMS    += mainwindow.ui

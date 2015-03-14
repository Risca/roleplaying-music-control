#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T15:32:14
#
#-------------------------------------------------

QT      += core gui multimedia
CONFIG  += console

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
    spotifyaudioworker.cpp \
    spotify_wrapper.cpp

HEADERS  += mainwindow.h \
    spotify.h \
    threadsafequeue.h \
    spotify_ll.h \
    spotifyaudioworker.h \
    spotify_wrapper.h

FORMS    += mainwindow.ui

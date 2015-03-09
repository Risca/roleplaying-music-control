#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T15:32:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = roleplaying-music-control
TEMPLATE = app

LIBS += -lspotify

SOURCES += main.cpp\
        mainwindow.cpp \
    spotify.cpp \
    appkey.c \
    spotify_ll.c

HEADERS  += mainwindow.h \
    spotify.h \
    threadsafequeue.h \
    spotify_ll.h

FORMS    += mainwindow.ui

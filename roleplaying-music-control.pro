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

LIBS += -lspotify -lzmq

SOURCES += main.cpp\
    spotify/appkey.c \
    spotify/spotify.cpp \
    spotify/spotify_ll.c \
    spotify/spotifyaudioworker.cpp \
    spotify/spotify_wrapper.cpp \
    logindialog.cpp \
    dmwindow.cpp \
    playerwindow.cpp

HEADERS  += \
    threadsafequeue.h \
    spotify/spotify.h \
    spotify/spotify_ll.h \
    spotify/spotifyaudioworker.h \
    spotify/spotify_wrapper.h \
    spotify/spotifytrackinfo.h \
    logindialog.h \
    dmwindow.h \
    playerwindow.h \
    zmqsubscriber.h

FORMS    += \
    logindialog.ui \
    dmwindow.ui \
    playerwindow.ui

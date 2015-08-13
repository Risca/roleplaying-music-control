#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T15:32:14
#
#-------------------------------------------------

QT      += core gui
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
    playerwindow.cpp \
    sdlaudiodriverchangedialog.cpp \
    sdlengine.cpp

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
    zmqsubscriber.h \
    3pp/cppzmq/zmq.hpp \
    sdlengine.h \
    sdlaudiodriverchangedialog.h

FORMS    += \
    logindialog.ui \
    dmwindow.ui \
    playerwindow.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2
win32 {
    contains(QMAKE_TARGET.arch, x86_64) {
        message("x86_64 windows build")
        LIBS += -L$$PWD/../../../../SDL2-2.0.3/x86_64-w64-mingw32/lib
        INCLUDEPATH += $$PWD/../../../../SDL2-2.0.3/x86_64-w64-mingw32/include
        DEPENDPATH += $$PWD/../../../../SDL2-2.0.3/x86_64-w64-mingw32/include
    } else {
        message("x86 windows build")
        LIBS += -L$$PWD/../../../../SDL2-2.0.3/i686-w64-mingw32/lib
        INCLUDEPATH += $$PWD/../../../../SDL2-2.0.3/i686-w64-mingw32/include
        DEPENDPATH += $$PWD/../../../../SDL2-2.0.3/i686-w64-mingw32/include
    }
    LIBS += -lmingw32 -lSDL2main -lSDL2 -mwindows
    QMAKE_CXXFLAGS += -Dmain=SDL_main

    LIBS += -L$$PWD/../../../../zeromq-4.1.2/.libs/ -llibzmq
    INCLUDEPATH += $$PWD/../../../../zeromq-4.1.2/include
    DEPENDPATH += $$PWD/../../../../zeromq-4.1.2/include

    LIBS += -L$$PWD/../../../../libspotify-12.1.51-win32-release/lib/ -llibspotify
    INCLUDEPATH += $$PWD/../../../../libspotify-12.1.51-win32-release/include
    DEPENDPATH += $$PWD/../../../../libspotify-12.1.51-win32-release/include
}


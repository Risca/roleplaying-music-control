#include "mainwindow.h"
#include <QApplication>

#include "spotify/spotifytrackinfo.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<SpotifyTrackInfo>();
    qRegisterMetaType<QList<SpotifyTrackInfo> >();
    MainWindow w;
    w.show();

    return a.exec();
}

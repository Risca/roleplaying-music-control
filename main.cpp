#include "mainwindow.h"
#include <QApplication>

#include "logindialog.h"
#include "spotify/spotifytrackinfo.h"


Spotify *login()
{
    LoginDialog login;
    int rv = login.exec();
    if (rv != QDialog::Accepted) {
        return 0;
    }
    return login.takeOverSpotifyContext();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<SpotifyTrackInfo>();
    qRegisterMetaType<QList<SpotifyTrackInfo> >();

    Spotify *spotify = login();
    if (0 == spotify) {
        return -1;
    }
    MainWindow w;
    w.setSpotifyContext(spotify);
    w.show();

    return a.exec();
}

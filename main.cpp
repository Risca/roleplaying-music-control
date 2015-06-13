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

    QCoreApplication::setOrganizationName("Risca hacks");
    QCoreApplication::setOrganizationDomain("risca.servebeer.com");
    QCoreApplication::setApplicationName("Roleplay Music Control");

    Spotify *spotify = login();
    if (0 == spotify) {
        return 0;
    }
    MainWindow w(spotify);
    w.show();

    return a.exec();
}

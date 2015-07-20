#include "dmwindow.h"
#include "playerwindow.h"
#include <QApplication>

#include "logindialog.h"
#include "spotify/spotifytrackinfo.h"


Spotify *login(bool &dm, QString &room)
{
    LoginDialog login;
    int rv = login.exec();
    if (rv != QDialog::Accepted) {
        return 0;
    }

    dm = login.isDungeonMaster();
    room = login.room();

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

    bool dm = false;
    QString room;
    Spotify *spotify = login(dm, room);
    if (0 == spotify) {
        return 0;
    }

    QScopedPointer<QWidget> main;
    if (dm) {
        main.reset(new DMWindow(spotify, room));
    }
    else {
        main.reset(new PlayerWindow(spotify));
    }
    main->show();

    return a.exec();
}

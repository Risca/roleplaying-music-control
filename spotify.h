#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QThread>

class QString;
class QStringList;

#ifdef __cplusplus
extern "C" {
struct sp_session;
}
#endif


class Spotify : public QThread
{
    Q_OBJECT

public:
    Spotify(const QString &username,
            const QString &password);
    ~Spotify();

signals:
    void loggedIn();
    void loggedOut();
    void playlistsUpdated(const QStringList &);

private:
    void run();
    void compileNewListOfPlaylists();

    QString user;
    QString pass;
    sp_session *sp;
};


#endif // SPOTIFY_H

#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QString>
#include <QThread>

#include "spotify_ll.h"
#include "threadsafequeue.h"

class QStringList;

#ifdef __cplusplus
extern "C" {
// Forward declarations
typedef struct sp_session sp_session;
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
    void logout();
    friend void eq_put(void * _obj, Event_t event);

    QString user;
    QString pass;
    sp_session *sp;
    ThreadSafeQueue<Event_t> eq;
};


#endif // SPOTIFY_H

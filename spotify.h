#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QAudioOutput>
#include <QBuffer>
#include <QMutex>
#include <QString>
#include <QThread>

#include "spotify_ll.h"
#include "threadsafequeue.h"

class QStringList;

#ifdef __cplusplus
extern "C" {
// Forward declarations
typedef struct sp_session sp_session;
typedef struct sp_audioformat sp_audioformat;
}
#endif


class Spotify : public QThread
{
    Q_OBJECT

public:
    Spotify(const QString &username,
            const QString &password);
    ~Spotify();

    int getNumChannels();
    int getSampleRate();
    qint64 readAudioData(char * data, int maxSize);

public slots:
    void playURI(const QString &URI);
    void changeCurrentlyPlayingSong();

signals:
    void loggedIn();
    void loggedOut();
    void songLoaded();
    void playlistsUpdated(const QStringList &);
    void newAudioDataReady();

private:
    void run();
    void compileNewListOfPlaylists();
    void logout();
    void setNumChannels(int newChannelCount);
    void setSampleRate(int newSampleRate);
    friend void eq_put(void * _obj, SpotifyEvent_t event);
    friend int music_delivery(void * obj,
                              sp_session *sess,
                              const sp_audioformat *format,
                              const void *frames,
                              int num_frames);

    QString user;
    QString pass;
    QString currentURI;
    QMutex accessMutex;
    QBuffer audioBuffer;
    int writePos, readPos;
    int numChannels;
    int sampleRate;
    sp_session *sp;
    ThreadSafeQueue<SpotifyEvent_t> eq;
    QThread audioThread;
};


#endif // SPOTIFY_H

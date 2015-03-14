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

    /* ---------------------------  SESSION CALLBACKS  ------------------------- */
    friend class Spotify_Wrapper;
    /**
     * This callback is called when an attempt to login has succeeded or failed.
     */
    void loggedInCb(sp_session *sp, sp_error err);

    /**
     * This callback is called when logout has been processed.
     */
    void loggedOutCb(sp_session *sp);

    void metadataUpdatedCb(sp_session *sp);

    /**
     * This callback is called from an internal libspotify thread to ask us to
     * reiterate the main loop.
     */
    void notifyMainThreadCb(sp_session *sess);

    /**
     * This callback is used from libspotify whenever there is PCM data available.
     */
    int musicDeliveryCb(sp_session *sess, const sp_audioformat *format,
                        const void *frames, int num_frames);

    /**
     * Called when the currently played track has reached its end
     */
    void endOfTrackCb(sp_session *sp);

    /**
     * This callback is called whenever an error occurs
     */
    void logErrorCb(sp_session *sp, sp_error err);

    /**
     * This callback is called whenever there is a message to log.
     */
    void logMessageCb(sp_session *sp, const char * data);


    /* ------------------- PLAYLIST CONTAINER ON CALLBACKS  -------------------- */
    /**
     * Called when a new playlist is added.
     */
    void playlistAddedCb(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata);

    /**
     * Called when a playlist is removed.
     */
    void playlistRemovedCb(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata);

    /**
     * Called when the playlist container is loaded
     */
    void playlistcontainerLoadedCb(sp_playlistcontainer * pc, void * userdata);


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

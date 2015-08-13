#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QBuffer>
#include <QList>
#include <QMutex>
#include <QSettings>
#include <QString>
#include <QThread>

#include "spotifytrackinfo.h"
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
    Spotify();
    ~Spotify();

    sp_audioformat getAudioFormat();
    qint64 readAudioData(char * data, qint64 maxSize);

    static QString songNameFromUri(const QString &uriString);

public slots:
    void login(const QString &username);
    void login(const QString &username, const QString &password);
    void playURI(const QString &URI);
    void changePlaylist(int idx);

signals:
    void loggedIn();
    void loggedOut();
    void songLoaded(const QString &uri);
    void playlistsUpdated(const QStringList &);
    void currentPlaylistUpdated(const QList<SpotifyTrackInfo> &);

private:
    void run();
    void compileNewListOfPlaylists();
    void changeCurrentlyPlayingSong();
    void changeCurrentPlaylist();
    void logout();
    void tryLoadTrack();
    void tryLoadPlaylist();
    QString uriFromTrack(sp_track * track);

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
     * Called when audio playback should start
     */
    void startPlaybackCb(sp_session *sp);

    /**
     * Called when audio playback should stop
     */
    void stopPlaybackCb(sp_session *sp);

    /**
     * Called to query application about its audio buffer
     */
    void getAudioBufferStatsCb(sp_session *sp, sp_audio_buffer_stats *stats);

    /**
     * Called when storable credentials have been updated, usually called when
     * we have connected to the AP.
     */
    void credentialsBlobUpdated(sp_session *sp, const char * data);

    /**
     * This callback is called whenever an error occurs
     */
    void logErrorCb(sp_session *sp, sp_error err);

    /**
     * This callback is called whenever there is a message to log.
     */
    void logMessageCb(sp_session *sp, const char * data);


    /* --------------------- PLAYLIST CONTAINER CALLBACKS  --------------------- */
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


    /* -------------------------- PLAYLIST CALLBACKS  -------------------------- */
    /**
     * Called when state changed for a playlist.
     */
    void playlistStateChangedCb(sp_playlist *pl, void *userdata);


    QString user;
    QString pass;
    QByteArray blob;
    QString currentURI;
    sp_track * nextTrack;
    sp_track * currentTrack;
    int currentPlaylistIdx;
    sp_playlist * currentPlaylist;
    QMutex accessMutex;
    QBuffer audioBuffer;
    int audioDropouts;
    qint64 writePos, readPos;
    sp_audioformat audioFormat;
    sp_session *sp;
    ThreadSafeQueue<SpotifyEvent_t> eq;
    QThread audioThread;
    QSettings settings;
};


#endif // SPOTIFY_H

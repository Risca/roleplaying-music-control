#include "spotify.h"
#include "spotify_ll.h"

#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include <QIODevice>
#include <QMutexLocker>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "spotifyaudioworker.h"
#include "spotify_wrapper.h"


#define BUFFER_SIZE 409600


Spotify::Spotify(const QString &username, const QString &password) :
    user(username),
    pass(password),
    writePos(0),
    readPos(0),
    numChannels(0),
    sampleRate(0),
    sp(0)
{
    Spotify_Wrapper::init(this);
}

Spotify::~Spotify()
{
    if (sp) {
        sp_session_release(sp);
    }
    audioThread.quit();
    audioThread.wait();
}

void Spotify::run()
{
    int next_timeout = 0;

    sp = spotify_ll_init(Spotify_Wrapper::sessionCallbacks());
    if (!sp) {
        return;
    }

    sp_session_login(sp,
                     user.toLocal8Bit().constData(),
                     pass.toLocal8Bit().constBegin(),
                     0, NULL);

    bool running = true;
    while (running) {
        SpotifyEvent_t ev;
        // Get next event (or timeout)
        if (next_timeout == 0) {
            eq.get(ev);
        } else {
            if (!eq.get(ev, next_timeout)) {
                // Timed out
                ev = EVENT_TIMEOUT;
            }
        }

        switch (ev) {
        case EVENT_SPOTIFY_MAIN_TICK:
        case EVENT_TIMEOUT:
            break;

        case EVENT_LOGGED_IN:
            emit loggedIn();
            break;

        case EVENT_PLAYLIST_CONTAINER_LOADED:
            compileNewListOfPlaylists();
            break;

        case EVENT_LOGGED_OUT:
            emit loggedOut();
            running = false;
            break;

        case EVENT_URI_CHANGED:
            changeCurrentlyPlayingSong();
            break;

        case EVENT_AUDIO_DATA_ARRIVED:
            if (!audioThread.isRunning()) {
                fprintf(stderr, "Spotify: Starting audio worker\n");

                SpotifyAudioWorker * audioWorker = new SpotifyAudioWorker(this);
                audioWorker->moveToThread(&audioThread);
                connect(this, SIGNAL(newAudioDataReady()),
                        audioWorker, SLOT(updateAudioBuffer()));
                connect(&audioThread, SIGNAL(started()),
                        audioWorker, SLOT(startStreaming()));
                connect(&audioThread, SIGNAL(finished()),
                        audioWorker, SLOT(deleteLater()));
                audioThread.start();
            }
            emit newAudioDataReady();
            break;

        case EVENT_END_OF_TRACK:
            sp_session_player_unload(sp);
            if (audioThread.isRunning()) {
                audioThread.quit();
                audioThread.wait();
            }
            break;

        default:
            qDebug() << "Unknown event:" << (int)ev;
            break;
        }

        do {
            sp_session_process_events(sp, &next_timeout);
        } while (next_timeout == 0);
    }
}

void Spotify::compileNewListOfPlaylists()
{
    sp_playlistcontainer * pc = sp_session_playlistcontainer(sp);
    int numPlaylists = sp_playlistcontainer_num_playlists(pc);

    QStringList names;
    for (int idx = 0; idx < numPlaylists; ++idx) {
        sp_playlist *pl = sp_playlistcontainer_playlist(pc, idx);
        names << QString::fromLocal8Bit(sp_playlist_name(pl));
    }

    emit playlistsUpdated(names);
}

void Spotify::changeCurrentlyPlayingSong()
{
    sp_error err;
    sp_track * track;
    const char * uri = currentURI.toLocal8Bit().constData();
    fprintf(stderr, "Spotify: Playing %s\n", uri);

    sp_link * link = sp_link_create_from_string(uri);
    if (!link) {
        qDebug() << "Failed to parse URI:" << currentURI;
        currentURI.clear();
        return;
    }

    switch (sp_link_type(link)) {
    case SP_LINKTYPE_LOCALTRACK:
    case SP_LINKTYPE_TRACK:
        track = sp_link_as_track(link);
        if (!track) {
            fprintf(stderr, "Link is not a track\n");
            break;
        }

        err = sp_session_player_load(sp, track);
        if (err != SP_ERROR_OK) {
            fprintf(stderr, "Failed to load URI (%s): 0x%02X %s\n", uri,
                    (unsigned)err, sp_error_message(err));
            break;
        }

        err = sp_session_player_play(sp, true);
        if (err != SP_ERROR_OK) {
            fprintf(stderr, "Failed to play URI (%s): %s\n", uri,
                    sp_error_message(err));
            break;
        }
        emit songLoaded();
        break;

    default:
        qDebug() << "URI is not a track:" << currentURI;
        break;
    }

    sp_link_release(link);
}

void Spotify::logout()
{
    sp_error err = sp_session_logout(sp);
    if (err != SP_ERROR_OK) {
        qDebug() << "Unable to logout:" << QString::fromLocal8Bit(sp_error_message(err));
    } else {
        qDebug() << "Logout request posted";
    }
}

void Spotify::setNumChannels(int newChannelCount)
{
    QMutexLocker locker(&accessMutex);
    numChannels = newChannelCount;
}

int Spotify::getNumChannels()
{
    QMutexLocker locker(&accessMutex);
    return numChannels;
}

int Spotify::getSampleRate()
{
    QMutexLocker locker(&accessMutex);
    return sampleRate;
}

qint64 Spotify::readAudioData(char *data, int maxSize)
{
    QMutexLocker locker(&accessMutex);
    int toRead = qMin(writePos - readPos, maxSize);
    audioBuffer.seek(readPos);
    int read =  audioBuffer.read(data, toRead);
    if (read < 0) {
        fprintf(stderr, "Spotify: Failed to read from audioBuffer\n");
    } else {
        readPos += read;
    }
    return read;
}

void Spotify::playURI(const QString &URI)
{
    QMutexLocker locker(&accessMutex);
    currentURI = URI;
    eq.put(EVENT_URI_CHANGED);
}

void Spotify::setSampleRate(int newSampleRate)
{
    QMutexLocker locker(&accessMutex);
    sampleRate = newSampleRate;
}

void Spotify::loggedInCb(sp_session *sp, sp_error err)
{
    if (err == SP_ERROR_OK) {
        sp_playlistcontainer_add_callbacks(sp_session_playlistcontainer(sp),
                                           Spotify_Wrapper::playlistcontainerCallbacks(),
                                           this);
        eq.put(EVENT_LOGGED_IN);
    } else {
        fprintf(stderr, "Failed to login: %s\n",
                sp_error_message(err));
        eq.put(EVENT_LOGGED_OUT);
    }
}

void Spotify::loggedOutCb(sp_session *sp)
{
    puts("Logged out!");
    eq.put(EVENT_LOGGED_OUT);
}

void Spotify::metadataUpdatedCb(sp_session *sp)
{
    eq.put(EVENT_METADATA_UPDATED);
}

void Spotify::notifyMainThreadCb(sp_session *sess)
{
    eq.put(EVENT_SPOTIFY_MAIN_TICK);
}

int Spotify::musicDeliveryCb(sp_session *, const sp_audioformat *format,
                             const void *frames, int num_frames)
{
    if (num_frames == 0) {
        fprintf(stderr, "No frames?\n");
        return 0;
    }

    QMutexLocker locker(&accessMutex);

    numChannels = format->channels;
    sampleRate = format->sample_rate;

    if (!audioBuffer.isOpen()) {
        audioBuffer.open(QIODevice::ReadWrite);
    }

    int availableFrames = (BUFFER_SIZE - (writePos - readPos)) / (sizeof(int16_t) * format->channels);
    int writtenFrames = qMin(num_frames, availableFrames);

    if (writtenFrames == 0) {
        goto out;
    }

    audioBuffer.seek(writePos);
    writePos += audioBuffer.write((const char *) frames, writtenFrames * sizeof(int16_t) * format->channels);

out:
    eq.put(EVENT_AUDIO_DATA_ARRIVED);
    return writtenFrames;
}

void Spotify::endOfTrackCb(sp_session *sp)
{
    eq.put(EVENT_END_OF_TRACK);
}

void Spotify::logErrorCb(sp_session *sp, sp_error err)
{
    fprintf(stderr, "An error occured: %s\n",
            sp_error_message(err));
}

void Spotify::logMessageCb(sp_session *sp, const char *data)
{
    char * str = strdup(data);
    // Remove (and truncare to) first newline
    str[strcspn(str, "\n")] = '\0';
    printf("%s\n", str);
    free(str);
}

void Spotify::playlistAddedCb(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
{
}

void Spotify::playlistRemovedCb(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
{
}

void Spotify::playlistcontainerLoadedCb(sp_playlistcontainer *, void *)
{
    eq.put(EVENT_PLAYLIST_CONTAINER_LOADED);
}

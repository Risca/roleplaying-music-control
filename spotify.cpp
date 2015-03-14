#include "spotify.h"
#include "spotify_ll.h"

#include <libspotify/api.h>
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include <QIODevice>
#include <QMutexLocker>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "spotifyaudioworker.h"


#define BUFFER_SIZE 409600


int music_delivery(void * obj,
                   sp_session *,
                   const sp_audioformat *format,
                   const void *frames,
                   int num_frames)
{
    if (num_frames == 0) {
        return 0;
    }

    Spotify * _this = static_cast<Spotify*>(obj);
    QMutexLocker locker(&_this->accessMutex);

    _this->numChannels = format->channels;
    _this->sampleRate = format->sample_rate;

    if (!_this->audioBuffer.isOpen()) {
        _this->audioBuffer.open(QIODevice::ReadWrite);
    }

    int availableFrames = (BUFFER_SIZE - (_this->writePos - _this->readPos)) / (sizeof(int16_t) * format->channels);
    int writtenFrames = qMin(num_frames, availableFrames);

    if (writtenFrames == 0)
        return 0;

    _this->audioBuffer.seek(_this->writePos);
    _this->writePos += _this->audioBuffer.write((const char *) frames, writtenFrames * sizeof(int16_t) * format->channels);

    return writtenFrames;
}


void eq_put(void * obj, SpotifyEvent_t event)
{
    Spotify * _this = static_cast<Spotify*>(obj);
    _this->eq.put(event);
}


Spotify::Spotify(const QString &username, const QString &password) :
    user(username),
    pass(password),
    writePos(0),
    readPos(0),
    numChannels(0),
    sampleRate(0),
    sp(0)
{
}

Spotify::~Spotify()
{
    if (sp) {
        sp_session_release(sp);
    }
    audioThread.quit();
    audioThread.wait();
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

void Spotify::run()
{
    int next_timeout = 0;

    sp = spotify_ll_init(this, eq_put, music_delivery);
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
            spotify_ll_setup_playlistcontainer(sp);
            break;

        case EVENT_PLAYLIST_CONTAINER_LOADED:
            compileNewListOfPlaylists();
//            logout();
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
                audioThread.start();
            }
            emit newAudioDataReady();
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
    fprintf(stderr, "Playing: %s\n", uri);

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
            fprintf(stderr, "Failed to load URI (%s): %s\n", uri,
                    sp_error_message(err));
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

void Spotify::setSampleRate(int newSampleRate)
{
    QMutexLocker locker(&accessMutex);
    sampleRate = newSampleRate;
}

int Spotify::getSampleRate()
{
    QMutexLocker locker(&accessMutex);
    return sampleRate;
}

#include "spotifyaudioworker.h"

#include <QDebug>
#include <QInputDialog>
#include <QIODevice>
#include <QTimer>
#include <QVector>
#include <cstdlib>

#include "spotify.h"


#define AUDIOSTREAM_UPDATE_INTERVAL 20
#define BUFFER_SIZE 409600


SpotifyAudioWorker::SpotifyAudioWorker(Spotify * spotify) :
    QObject(0),
    spotify(spotify),
    dev(0)
{
}

SpotifyAudioWorker::~SpotifyAudioWorker()
{
    if (dev) {
        SDL_CloseAudioDevice(dev);
    }
    fprintf(stderr, "SpotifyAudioWorker:~SpotifyAudioWorker\n");
}

void SpotifyAudioWorker::startStreaming()
{
    SDL_AudioSpec want, have;

    SDL_zero(want);
    sp_audioformat format = spotify->getAudioFormat();
    switch (format.sample_type) {
    case SP_SAMPLETYPE_INT16_NATIVE_ENDIAN:
        want.format = AUDIO_S16SYS;
        break;
    default:
        return;
    }
    want.freq = format.sample_rate;
    want.channels = format.channels;
    want.samples = 4096;
    want.callback = SDLAudioCb;
    want.userdata = this;

    QString deviceName = settings.value("SDL/AudioDevice").toString();
    if (deviceName.isEmpty()) {
        dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    }
    else {
        dev = SDL_OpenAudioDevice(deviceName.toStdString().c_str(), 0, &want, &have, 0);
    }
    if (dev == 0) {
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
        emit streamingFailed();
    } else {
        SDL_PauseAudioDevice(dev, 0);
    }
}

void SpotifyAudioWorker::stopStreaming()
{
    if (dev) {
        SDL_PauseAudioDevice(dev, 1);
    }
}

void SpotifyAudioWorker::SDLAudioCb(void *userdata, Uint8 *stream, int len)
{
    SpotifyAudioWorker * worker = reinterpret_cast<SpotifyAudioWorker*>(userdata);
    if (worker) {
        worker->audioCb(stream, len);
    }
}

void SpotifyAudioWorker::audioCb(Uint8 *stream, int len)
{
    spotify->readAudioData((char*)stream, len);
}

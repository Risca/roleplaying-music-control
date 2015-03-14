#include "spotifyaudioworker.h"

#include <QAudioOutput>
#include <QDebug>
#include <QIODevice>
#include <QTimer>

#include "spotify.h"


#define AUDIOSTREAM_UPDATE_INTERVAL 20
#define BUFFER_SIZE 409600


SpotifyAudioWorker::SpotifyAudioWorker(Spotify * spotify) :
    QObject(0),
    spotify(spotify),
    audioIODevice(0),
    ao(0)
{
}

SpotifyAudioWorker::~SpotifyAudioWorker()
{
    if (ao) {
        ao->stop();
    }
    delete ao;
    fprintf(stderr, "SpotifyAudioWorker:~SpotifyAudioWorker\n");
}

void SpotifyAudioWorker::startStreaming()
{
    if (!ao) {
        QAudioFormat af;
        af.setChannelCount(spotify->getNumChannels());
        af.setCodec("audio/pcm");
        af.setSampleRate(spotify->getSampleRate());
        af.setSampleSize(16);
        af.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(af)) {
            qWarning() << "raw audio format not supported by backend, cannot play audio.";
            return;
        }

        ao = new QAudioOutput(af);
        ao->setBufferSize(BUFFER_SIZE);
        connect(ao, SIGNAL(stateChanged(QAudio::State)),
                this, SLOT(handleStateChanged(QAudio::State)));
        audioIODevice = ao->start();
        ao->suspend();

        QTimer * timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateAudioBuffer()));
        timer->start(AUDIOSTREAM_UPDATE_INTERVAL);

        fprintf(stderr, "SpotifyAudioWorker: Start audio streaming\n");
    }
}

void SpotifyAudioWorker::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        fprintf(stderr, "SpotifyAudioWorker: Finished playing (no more data)\n");
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        fprintf(stderr, "SpotifyAudioWorker: entered stopped state\n");
        if (ao->error() != QAudio::NoError) {
            fprintf(stderr, "SpotifyAudioWorker: error code %d\n",
                    (int)ao->error());
        }
        break;

    case QAudio::ActiveState:
        fprintf(stderr, "SpotifyAudioWorker: entered active state\n");
        break;

    case QAudio::SuspendedState:
        fprintf(stderr, "SpotifyAudioWorker: entered suspended state\n");
        break;
    }
}

void SpotifyAudioWorker::updateAudioBuffer()
{
    if (!ao)
        return;

    if (ao->state() == QAudio::SuspendedState) {
        ao->resume();
    }

    int bytesFree = ao->bytesFree();
    char data[bytesFree];
    int bytesRead = spotify->readAudioData(data, bytesFree);
    if (bytesRead > 0) {
        audioIODevice->write(data, bytesRead);
    }

    static int count = 0;
    if ((count++ % 100) == 0) {
        fprintf(stderr, "SpotifyAudioWorker: Updating audio buffer (%d)\n",
                bytesRead);
    }
}

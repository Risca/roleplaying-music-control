#include "spotifyaudioworker.h"

#include <QAudioOutput>
#include <QDebug>
#include <QIODevice>

#include "spotify.h"


#define BUFFER_SIZE 409600


SpotifyAudioWorker::SpotifyAudioWorker(Spotify * spotify) :
    QObject(spotify),
    spotify(spotify),
    audioIODevice(0),
    ao(0)
{
}

SpotifyAudioWorker::~SpotifyAudioWorker()
{
    delete audioIODevice;
    delete ao;
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
        audioIODevice = ao->start();
        ao->suspend();
        fprintf(stderr, "Start audio streaming\n");
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
    audioIODevice->write(data, bytesRead);

    static int count = 0;
    if ((++count % 100) == 0) {
        fprintf(stderr, "Updating audio buffer (%d)\n", bytesRead);
        qDebug() << "updating audio buffer";
    }

}

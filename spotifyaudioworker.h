#ifndef SPOTIFYAUDIOWORKER_H
#define SPOTIFYAUDIOWORKER_H

#include <QObject>
#include <QTimer>

class QAudioOutput;
class QIODevice;
class Spotify;


class SpotifyAudioWorker : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyAudioWorker(Spotify * spotify);
    ~SpotifyAudioWorker();

public slots:
    void startStreaming();
    void updateAudioBuffer();

private:
    Spotify * spotify;
    QIODevice * audioIODevice;
    QAudioOutput * ao;

};

#endif // SPOTIFYAUDIOWORKER_H

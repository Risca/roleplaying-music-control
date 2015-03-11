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

private:
    Spotify * spotify;
    QIODevice * audioIODevice;
    QAudioOutput * ao;

private slots:
    void updateAudioBuffer();
};

#endif // SPOTIFYAUDIOWORKER_H

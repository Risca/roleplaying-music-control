#ifndef SPOTIFYAUDIOWORKER_H
#define SPOTIFYAUDIOWORKER_H

#include <QObject>
#include <QTimer>
#include <SDL2/SDL.h>

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
    void stopStreaming();

private:
    Spotify * spotify;
    SDL_AudioDeviceID dev;

    static void SDLCALL SDLAudioCb (void *userdata, Uint8 * stream, int len);
    void audioCb(Uint8 * stream, int len);
};

#endif // SPOTIFYAUDIOWORKER_H

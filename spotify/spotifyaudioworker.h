#ifndef SPOTIFYAUDIOWORKER_H
#define SPOTIFYAUDIOWORKER_H

#include <QObject>
#include <QTimer>
#include <vlc/vlc.h>

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
    libvlc_instance_t * vlc;
    libvlc_media_player_t *mediaPlayer;
    int64_t ts;

    static int ImemGetCb (void *data,
                          const char *cookie,
                          int64_t *dts,
                          int64_t *pts,
                          unsigned *flags,
                          size_t * bufferSize,
                          void ** buffer);
    int ImemGet (int64_t *dts,
                 int64_t *pts,
                 unsigned *flags,
                 size_t * bufferSize,
                 void ** buffer);
    static int ImemReleaseCb (void *data,
                              const char *cookie,
                              size_t bufferSize,
                              void * buffer);
    int ImemRelease (size_t bufferSize, void *buffer);
};

#endif // SPOTIFYAUDIOWORKER_H

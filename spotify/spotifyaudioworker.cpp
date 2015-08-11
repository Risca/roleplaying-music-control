#include "spotifyaudioworker.h"

#include <QDebug>
#include <QIODevice>
#include <QTimer>
#include <QVector>

#include "spotify.h"


#define AUDIOSTREAM_UPDATE_INTERVAL 20
#define BUFFER_SIZE 409600


SpotifyAudioWorker::SpotifyAudioWorker(Spotify * spotify) :
    QObject(0),
    spotify(spotify),
    vlc(0),
    mediaPlayer(0),
    ts(0)
{
}

SpotifyAudioWorker::~SpotifyAudioWorker()
{
    if (mediaPlayer) {
        libvlc_media_player_stop (mediaPlayer);
        libvlc_media_player_release (mediaPlayer);
        mediaPlayer = 0;
    }

    if (vlc) {
        libvlc_release (vlc);
        vlc = 0;
    }
    fprintf(stderr, "SpotifyAudioWorker:~SpotifyAudioWorker\n");
}

void SpotifyAudioWorker::startStreaming()
{
    libvlc_media_t *media;
    QVector<const char*> options;

    QByteArray dataArg = "--imem-data=" + QByteArray::number((qulonglong)this);
    options.push_back(dataArg.constData());

    QByteArray getArg = "--imem-get=" + QByteArray::number((qulonglong)ImemGetCb);
    options.push_back(getArg.constData());

    QByteArray releaseArg = "--imem-release=" + QByteArray::number((qulonglong)ImemReleaseCb);
    options.push_back(releaseArg.constData());

    QByteArray sampleRateArg = "--imem-samplerate=" + QByteArray::number(spotify->getSampleRate());
    options.push_back(sampleRateArg.constData());

    QByteArray channelsArg = "--imem-channels=" + QByteArray::number(spotify->getNumChannels());
    options.push_back(channelsArg.constData());

    options.push_back("--imem-cookie=\"RMC\"");
    options.push_back("--imem-codec=s16l");
    options.push_back("--imem-cat=1");
    //options.push_back("--no-video-title-show");
    //options.push_back("--verbose=2");

    fprintf(stderr, "SpotifyAudioWorker: options:");
    foreach (const char *str, options) {
        fprintf(stderr, " %s", str);
    }
    fputs("\n", stderr);

    // Load the VLC engine
    vlc = libvlc_new (int(options.size()), options.data());

    // Create a media item from file
    media = libvlc_media_new_location (vlc, "imem://");

    // Create a media player playing environment
    mediaPlayer = libvlc_media_player_new_from_media (media);

    // No need to keep the media now
    libvlc_media_release (media);

    // play the media_player
    ts = 0;
    libvlc_media_player_play (mediaPlayer);
}

/**
    \brief Callback method triggered by VLC to get data from a custom memory
    source. This is used to tell VLC where the data is and to allocate buffers
    as needed.

    To set this callback, use the "--imem-get=<memory_address>"
    option, with memory_address the address of this function in memory.

    When using IMEM, be sure to indicate the format for your data
    using "--imem-cat=2" where 2 is video. Other options for categories are
    0 = Unknown,
    1 = Audio,
    2 = Video,
    3 = Subtitle,
    4 = Data

    When creating your media instance, use libvlc_media_new_location and
    set the location to "imem://" and then play.

    \param[in] data Pointer to user-defined data, this is your data that
    you set by passing the "--imem-data=<memory_address>" option when
    initializing VLC instance.
    \param[in] cookie A user defined string. This works the same way as
    data, but for string. You set it by adding the "--imem-cookie=<your_string>"
    option when you initialize VLC. Use this when multiple VLC instances are
    running.
    \param[out] dts The decode timestamp, value is in microseconds. This value
    is the time when the frame was decoded/generated. For example, 30 fps
    video would be every 33 ms, so values would be 0, 33333, 66666, 99999, etc.
    \param[out] pts The presentation timestamp, value is in microseconds. This
    value tells the receiver when to present the frame. For example, 30 fps
    video would be every 33 ms, so values would be 0, 33333, 66666, 99999, etc.
    \param[out] flags Unused,ignore.
    \param[out] bufferSize Use this to set the size of the buffer in bytes.
    \param[out] buffer Change to point to your encoded frame/audio/video data.
        The codec format of the frame is user defined and set using the
        "--imem-codec=<four_letter>," where 4 letter is the code for your
        codec of your source data.
*/
int SpotifyAudioWorker::ImemGetCb(void *data,
                                  const char *,
                                  int64_t *dts,
                                  int64_t *pts,
                                  unsigned *flags,
                                  size_t *bufferSize,
                                  void **buffer)
{
    SpotifyAudioWorker * _this = reinterpret_cast<SpotifyAudioWorker*>(data);
    if (_this) {
        return _this->ImemGet(dts, pts, flags, bufferSize, buffer);
    }
    return 0;
}

int SpotifyAudioWorker::ImemGet(int64_t *dts, int64_t *pts, unsigned *, size_t *bufferSize, void **buffer)
{
    size_t size = sizeof(int16_t) * spotify->getNumChannels();
    char * data = new char[size];
    if (spotify->readAudioData(data, size) != (qint64)size) {
        // TODO: sleep while waiting for new data
        delete [] data;
        return 0;
    }
    ts += 1000000/spotify->getSampleRate();

    *bufferSize = size;
    *buffer = data;
    *dts = ts;
    *pts = ts;

    return 0;
}


/**
    \brief Callback method triggered by VLC to release memory allocated
    during the GET callback.

    To set this callback, use the "--imem-release=<memory_address>"
    option, with memory_address the address of this function in memory.

    \param[in] data Pointer to user-defined data, this is your data that
    you set by passing the "--imem-data=<memory_address>" option when
    initializing VLC instance.
    \param[in] cookie A user defined string. This works the same way as
    data, but for string. You set it by adding the "--imem-cookie=<your_string>"
    option when you initialize VLC. Use this when multiple VLC instances are
    running.
    \param[int] bufferSize The size of the buffer in bytes.
    \param[out] buffer Pointer to data you allocated or set during the GET
    callback to handle  or delete as needed.
*/
int SpotifyAudioWorker::ImemReleaseCb(void *data,
                                      const char *,
                                      size_t bufferSize,
                                      void *buffer)
{
    SpotifyAudioWorker * _this = reinterpret_cast<SpotifyAudioWorker*>(data);
    if (_this) {
        _this->ImemRelease(bufferSize, static_cast<char *>(buffer));
    }
    return 0;
}

int SpotifyAudioWorker::ImemRelease(size_t, void *buffer)
{
    delete [] static_cast<char*>(buffer);
    return 0;
}

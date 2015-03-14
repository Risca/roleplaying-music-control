#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdbool.h>

// Forward declarations
typedef struct sp_session sp_session;
typedef struct sp_audioformat sp_audioformat;


typedef enum {
    EVENT_NO_EVENT,
    EVENT_TIMEOUT,
    EVENT_SPOTIFY_MAIN_TICK,
    EVENT_LOGGED_IN,
    EVENT_LOGGED_OUT,
    EVENT_PLAYLIST_CONTAINER_LOADED,
    EVENT_URI_CHANGED,
    EVENT_AUDIO_DATA_ARRIVED,
    EVENT_END_OF_TRACK,
} SpotifyEvent_t;


sp_session * spotify_ll_init(void * obj, void (*queue_put_cb)(void *, SpotifyEvent_t),
                             int (*music_callback)(void *, sp_session *, const sp_audioformat *, const void *, int));
void spotify_ll_setup_playlistcontainer(sp_session * sp);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SPOTIFY_LL_H

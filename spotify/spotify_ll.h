#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <libspotify/api.h>
#include <stdbool.h>


typedef enum {
    EVENT_NO_EVENT,
    EVENT_TIMEOUT,
    EVENT_SPOTIFY_MAIN_TICK,
    EVENT_LOGGED_IN,
    EVENT_LOGGED_OUT,
    EVENT_PLAYLIST_CONTAINER_LOADED,
    EVENT_URI_CHANGED,
    EVENT_PLAYLIST_IDX_CHANGED,
    EVENT_START_PLAYBACK,
    EVENT_STOP_PLAYBACK,
    EVENT_END_OF_TRACK,
    EVENT_METADATA_UPDATED,
    EVENT_LOGIN_CREDENTIALS_CHANGED
} SpotifyEvent_t;


sp_session * spotify_ll_init(sp_session_callbacks *session_callbacks);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SPOTIFY_LL_H

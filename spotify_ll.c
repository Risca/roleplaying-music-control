#include "spotify_ll.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <libspotify/api.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* --- Data --- */
extern const uint8_t g_appkey[];
extern const size_t g_appkey_size;

void (*eq_put)(void *, Event_t) = NULL;
void * _obj = NULL;


/* ------------------- PLAYLIST CONTAINER ON CALLBACKS  -------------------- */
static void playlist_added(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata)
{
}


static void playlist_removed(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata)
{
}


static void playlistcontainer_loaded(sp_playlistcontainer * pc, void * userdata)
{
    eq_put(_obj, EVENT_PLAYLIST_CONTAINER_LOADED);
}

/**
 * The playlist container callbacks
 */
static sp_playlistcontainer_callbacks pc_callbacks = {
    .playlist_added = &playlist_added,
    .playlist_removed = &playlist_removed,
    .container_loaded = &playlistcontainer_loaded,
};


/* ---------------------------  SESSION CALLBACKS  ------------------------- */
/**
 * This callback is called when an attempt to login has succeeded or failed.
 *
 * @sa sp_session_callbacks#logged_in
 */
static void logged_in(sp_session *sp, sp_error err)
{
    if (err == SP_ERROR_OK) {
        printf("Logged in! Loggin out.\n");
        eq_put(_obj, EVENT_LOGGED_IN);
    } else {
        fprintf(stderr, "Failed to login: %s\n",
                sp_error_message(err));
        eq_put(_obj, EVENT_LOGGED_OUT);
    }
}

/**
 * This callback is called when logout has been processed.
 *
 * @sa sp_session_callbacks#logged_out
 */
static void logged_out(sp_session *sp)
{
    puts("Logged out!");
    eq_put(_obj, EVENT_LOGGED_OUT);
}

/**
 * This callback is called from an internal libspotify thread to ask us to
 * reiterate the main loop.
 *
 * We notify the main thread using a condition variable and a protected variable.
 *
 * @sa sp_session_callbacks#notify_main_thread
 */
static void notify_main_thread(sp_session *sess)
{
    eq_put(_obj, EVENT_SPOTIFY_MAIN_TICK);
}

/**
 * This callback is called whenever an error occurs
 *
 * @sa sp_session_callbacks#logged_out
 */
static void log_error(sp_session *sp, sp_error err)
{
    fprintf(stderr, "An error occured: %s\n",
            sp_error_message(err));
}

/**
 * This callback is called whenever there is a message to log.
 *
 * @sa sp_session_callbacks#logged_out
 */
static void log_message(sp_session *sp, const char * data)
{
    char * str = strdup(data);
    // Remove (and truncare to) first newline
    str[strcspn(str, "\n")] = '\0';
    printf("%s\n", str);
    free(str);
}

/**
 * The session callbacks
 */
static void dummy(sp_session *sp) {}
static sp_session_callbacks session_callbacks = {
    .logged_in = &logged_in,
    .logged_out = &logged_out,
    .metadata_updated = &dummy,
    .connection_error = &log_error,
    .message_to_user = &log_message,
    .notify_main_thread = &notify_main_thread,
    .music_delivery = NULL,
    .play_token_lost = &dummy,
    .log_message = &log_message,
    .end_of_track = &dummy,
    .streaming_error = &log_error,
    .userinfo_updated = &dummy,
    .start_playback = &dummy,
    .stop_playback = &dummy,
    .get_audio_buffer_stats = NULL,
    .offline_status_updated = &dummy,
    .offline_error = &log_error,
    .credentials_blob_updated = &log_message,
    .connectionstate_updated = &dummy,
    .scrobble_error = &log_error,
    .private_session_mode_changed = NULL
};


/**
 * The session configuration. Note that application_key_size is an external, so
 * we set it in main() instead.
 */
static sp_session_config spconfig = {
    .api_version = SPOTIFY_API_VERSION,
    .cache_location = "tmp",
    .settings_location = "tmp",
    .application_key = g_appkey,
    .application_key_size = 0, // Set in main()
    .user_agent = "roleplaying-music-control",
    .callbacks = &session_callbacks,
    NULL,
};


sp_session * spotify_ll_init(void * obj, void (*queue_put_cb)(void *, Event_t))
{
    sp_error err;
    sp_session *sp;

    if (!queue_put_cb) {
        return NULL;
    }
    _obj = obj;
    eq_put = queue_put_cb;

    spconfig.application_key_size = g_appkey_size;

    err = sp_session_create(&spconfig, &sp);
    if (err != SP_ERROR_OK) {
        fprintf(stderr, "Unable to create session: %s\n",
                sp_error_message(err));
        return NULL;
    }

    return sp;
}


void spotify_ll_setup_playlistcontainer(sp_session *sp)
{
    sp_playlistcontainer * pc = sp_session_playlistcontainer(sp);
    sp_playlistcontainer_add_callbacks(pc, &pc_callbacks, NULL);
}

#ifdef __cplusplus
}
#endif // __cplusplus

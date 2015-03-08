#include "spotify.h"

#include <libspotify/api.h>
#include <QString>
#include <QStringList>
#include <stdio.h>
#include <stdint.h>

#include "threadsafequeue.h"

static ThreadSafeQueue<int> eq;
static void eq_put(int event)
{
    eq.put(event);
}
static int eq_get(int next_timeout = 0)
{
    int event;

    if (next_timeout != 0) {
        if (!eq.get(event, next_timeout)) {
            event = 1; // Timeout
        }
    } else {
        eq.get(event);
    }

    return event;
}

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EVENT_NO_EVENT,
    EVENT_TIMEOUT,
    EVENT_SPOTIFY_MAIN_TICK,
    EVENT_LOGGED_IN,
    EVENT_LOGGED_OUT,
    EVENT_PLAYLIST_CONTAINER_LOADED,
} Event_t;

/* --- Data --- */
extern const uint8_t g_appkey[];
extern const size_t g_appkey_size;

static void logout(sp_session *sp)
{
    sp_error err = sp_session_logout(sp);
    if (err != SP_ERROR_OK) {
        fprintf(stderr, "Unable to logout: %s\n",
                sp_error_message(err));
    } else {
        puts("Logout request posted");
    }
}


/* ------------------- PLAYLIST CONTAINER ON CALLBACKS  -------------------- */
static void playlist_added(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata)
{
}


static void playlist_removed(sp_playlistcontainer * pc, sp_playlist * playlist, int position, void * userdata)
{
}


static void playlistcontainer_loaded(sp_playlistcontainer * pc, void * userdata)
{
    eq_put(EVENT_PLAYLIST_CONTAINER_LOADED);
}

/**
 * The playlist container callbacks
 */
static sp_playlistcontainer_callbacks pc_callbacks;

#ifdef __cplusplus
}
#endif


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
        eq_put(EVENT_LOGGED_IN);
    } else {
        fprintf(stderr, "Failed to login: %s\n",
                sp_error_message(err));
        eq_put(EVENT_LOGGED_OUT);
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
    eq_put(EVENT_LOGGED_OUT);
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
    eq_put(EVENT_SPOTIFY_MAIN_TICK);
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
static sp_session_callbacks session_callbacks;


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


Spotify::Spotify(const QString &username, const QString &password) :
    user(username), pass(password), sp(0)
{
    session_callbacks.logged_in = &logged_in;
    session_callbacks.logged_out = &logged_out;
    session_callbacks.metadata_updated = &dummy;
    session_callbacks.connection_error = &log_error;
    session_callbacks.message_to_user = &log_message;
    session_callbacks.notify_main_thread = &notify_main_thread;
    session_callbacks.music_delivery = NULL;
    session_callbacks.play_token_lost = &dummy;
    session_callbacks.log_message = &log_message;
    session_callbacks.end_of_track = &dummy;
    session_callbacks.streaming_error = &log_error;
    session_callbacks.userinfo_updated = &dummy;
    session_callbacks.start_playback = &dummy;
    session_callbacks.stop_playback = &dummy;
    session_callbacks.get_audio_buffer_stats = NULL;
    session_callbacks.offline_status_updated = &dummy;
    session_callbacks.offline_error = &log_error;
    session_callbacks.credentials_blob_updated = &log_message;
    session_callbacks.connectionstate_updated = &dummy;
    session_callbacks.scrobble_error = &log_error;
    session_callbacks.private_session_mode_changed = NULL;
    pc_callbacks.playlist_added = &playlist_added;
    pc_callbacks.playlist_removed = &playlist_removed;
    pc_callbacks.container_loaded = &playlistcontainer_loaded;
}

Spotify::~Spotify()
{
    if (sp) {
        sp_session_release(sp);
    }
}

void Spotify::run()
{
    sp_error err;
    int next_timeout = 0;

    spconfig.application_key_size = g_appkey_size;

    err = sp_session_create(&spconfig, &sp);
    if (err != SP_ERROR_OK) {
        fprintf(stderr, "Unable to create session: %s\n",
                sp_error_message(err));
        return;
    }

    sp_session_login(sp,
                     user.toLocal8Bit().constData(),
                     pass.toLocal8Bit().constBegin(),
                     0, NULL);

    bool running = true;
    while (running) {
        Event_t ev;
        if (next_timeout == 0) {
            ev = static_cast<Event_t>(eq_get());
        } else {
            ev = static_cast<Event_t>(eq_get(next_timeout));
        }

        switch (ev) {
        case EVENT_SPOTIFY_MAIN_TICK:
        case EVENT_TIMEOUT:
            break;

        case EVENT_LOGGED_IN:
            sp_playlistcontainer_add_callbacks(sp_session_playlistcontainer(sp), &pc_callbacks, NULL);
            break;

        case EVENT_PLAYLIST_CONTAINER_LOADED:
            compileNewListOfPlaylists();
            logout(sp);
            break;

        case EVENT_LOGGED_OUT:
            running = false;
            break;

        default:
            fprintf(stderr, "Unknown event (0x%02X)!\n", ev);
            running = false;
            break;
        }

        do {
            sp_session_process_events(sp, &next_timeout);
        } while (next_timeout == 0);
    }
}

void Spotify::compileNewListOfPlaylists()
{
    sp_playlistcontainer * pc = sp_session_playlistcontainer(sp);
    int numPlaylists = sp_playlistcontainer_num_playlists(pc);

    QStringList names;
    for (int idx = 0; idx < numPlaylists; ++idx) {
        sp_playlist *pl = sp_playlistcontainer_playlist(pc, idx);
        names << QString::fromLocal8Bit(sp_playlist_name(pl));
    }

    emit playlistsUpdated(names);
}

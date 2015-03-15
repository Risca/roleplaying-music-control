#ifndef SPOTIFY_WRAPPERS_H
#define SPOTIFY_WRAPPERS_H

#include <libspotify/api.h>

// Forward declarations
class Spotify;


class Spotify_Wrapper
{
public:
    // Run init() before any other functions
    static void init(Spotify * spotifyPointer);
    static sp_session_callbacks * sessionCallbacks();
    static sp_playlistcontainer_callbacks * playlistcontainerCallbacks();
    static sp_playlist_callbacks * playlistCallbacks();

private:
    Spotify_Wrapper() {}
    static sp_session_callbacks spCallbacks;
    static sp_playlistcontainer_callbacks pcCallbacks;
    static sp_playlist_callbacks plCallbacks;
    static Spotify * s;

    /**
     * Session callbacks
     */
    static void logged_in(sp_session *sp, sp_error err);
    static void logged_out(sp_session *sp);
    static void metadata_updated(sp_session *sp);
    static void notify_main_thread(sp_session *sp);
    static int music_delivery(sp_session *sp, const sp_audioformat *format,
                              const void *frames, int num_frames);
    static void end_of_track(sp_session *sp);
    static void start_playback(sp_session *sp);
    static void stop_playback(sp_session *sp);
    static void get_audio_buffer_stats(sp_session *sp, sp_audio_buffer_stats *stats);
    static void log_error(sp_session *sp, sp_error err);
    static void log_message(sp_session *sp, const char * data);
    static void dummy(sp_session*) {}

    /**
     * Playlist container callbacks
     */
    static void playlist_added(sp_playlistcontainer * pc, sp_playlist * playlist,
                               int position, void * userdata);
    static void playlist_removed(sp_playlistcontainer * pc, sp_playlist * playlist,
                                 int position, void * userdata);
    static void playlistcontainer_loaded(sp_playlistcontainer * pc, void * userdata);

    /**
     * Playlist callbacks
     */
    static void playlist_state_changed(sp_playlist *pl, void *userdata);
};

#endif // SPOTIFY_WRAPPERS_H

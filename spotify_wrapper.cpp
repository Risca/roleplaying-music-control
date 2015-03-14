#include "spotify_wrapper.h"

#include <libspotify/api.h>

#include "spotify.h"


// Static variables
Spotify * Spotify_Wrapper::s = 0;
sp_session_callbacks Spotify_Wrapper::spCallbacks;
sp_playlistcontainer_callbacks Spotify_Wrapper::pcCallbacks;


void Spotify_Wrapper::init(Spotify *spotifyPointer)
{
    s = spotifyPointer;
    spCallbacks.logged_in = &logged_in;
    spCallbacks.logged_out = &logged_out;
    spCallbacks.metadata_updated = &metadata_updated;
    spCallbacks.connection_error = &log_error;
    spCallbacks.message_to_user = &log_message;
    spCallbacks.notify_main_thread = &notify_main_thread;
    spCallbacks.music_delivery = &music_delivery;
    spCallbacks.play_token_lost = &dummy;
    spCallbacks.log_message = &log_message;
    spCallbacks.end_of_track = &end_of_track;
    spCallbacks.streaming_error = &log_error;
    spCallbacks.userinfo_updated = &dummy;
    spCallbacks.start_playback = &dummy;
    spCallbacks.stop_playback = &dummy;
    spCallbacks.get_audio_buffer_stats = 0;
    spCallbacks.offline_status_updated = &dummy;
    spCallbacks.offline_error = &log_error;
    spCallbacks.credentials_blob_updated = &log_message;
    spCallbacks.connectionstate_updated = &dummy;
    spCallbacks.scrobble_error = &log_error;
    spCallbacks.private_session_mode_changed = 0;

    pcCallbacks.playlist_added = &playlist_added;
    pcCallbacks.playlist_removed = &playlist_removed;
    pcCallbacks.container_loaded = &playlistcontainer_loaded;
}

sp_session_callbacks *Spotify_Wrapper::sessionCallbacks()
{
    if (s == 0) {
        return 0;
    }
    return &spCallbacks;
}

sp_playlistcontainer_callbacks *Spotify_Wrapper::playlistcontainerCallbacks()
{
    if (s == 0) {
        return 0;
    }
    return &pcCallbacks;
}

/* ---------------------------  SESSION CALLBACKS  ------------------------- */
void Spotify_Wrapper::logged_in(sp_session *sp, sp_error err)
{
    s->loggedInCb(sp, err);
}

void Spotify_Wrapper::logged_out(sp_session *sp)
{
    s->loggedOutCb(sp);
}

void Spotify_Wrapper::metadata_updated(sp_session *sp)
{
    s->metadataUpdatedCb(sp);
}

void Spotify_Wrapper::notify_main_thread(sp_session *sp)
{
    s->notifyMainThreadCb(sp);
}

int Spotify_Wrapper::music_delivery(sp_session *sp, const sp_audioformat *format,
                                    const void *frames, int num_frames)
{
    return s->musicDeliveryCb(sp, format, frames, num_frames);
}

void Spotify_Wrapper::end_of_track(sp_session *sp)
{
    s->endOfTrackCb(sp);
}

void Spotify_Wrapper::log_error(sp_session *sp, sp_error err)
{
    s->logErrorCb(sp, err);
}

void Spotify_Wrapper::log_message(sp_session *sp, const char * data)
{
    s->logMessageCb(sp, data);
}

/* ------------------- PLAYLIST CONTAINER ON CALLBACKS  -------------------- */
void Spotify_Wrapper::playlist_added(sp_playlistcontainer *pc, sp_playlist *playlist,
                                     int position, void *userdata)
{
    s->playlistAddedCb(pc, playlist, position, userdata);
}

void Spotify_Wrapper::playlist_removed(sp_playlistcontainer *pc, sp_playlist *playlist,
                                       int position, void *userdata)
{
    s->playlistRemovedCb(pc, playlist, position, userdata);
}

void Spotify_Wrapper::playlistcontainer_loaded(sp_playlistcontainer *pc, void *userdata)
{
    s->playlistcontainerLoadedCb(pc, userdata);
}

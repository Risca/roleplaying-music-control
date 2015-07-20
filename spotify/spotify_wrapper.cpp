#include "spotify_wrapper.h"

#include <libspotify/api.h>

#include "spotify.h"


// Static variables
Spotify * Spotify_Wrapper::s = 0;
sp_session_callbacks Spotify_Wrapper::spCallbacks;
sp_playlistcontainer_callbacks Spotify_Wrapper::pcCallbacks;
sp_playlist_callbacks Spotify_Wrapper::plCallbacks;


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
    spCallbacks.start_playback = &start_playback;
    spCallbacks.stop_playback = &stop_playback;
    spCallbacks.get_audio_buffer_stats = &get_audio_buffer_stats;
    spCallbacks.offline_status_updated = &dummy;
    spCallbacks.offline_error = &log_error;
    spCallbacks.credentials_blob_updated = &credentials_blob_updated;
    spCallbacks.connectionstate_updated = &dummy;
    spCallbacks.scrobble_error = &log_error;
    spCallbacks.private_session_mode_changed = 0;

    pcCallbacks.playlist_added = &playlist_added;
    pcCallbacks.playlist_removed = &playlist_removed;
    pcCallbacks.playlist_moved = 0;
    pcCallbacks.container_loaded = &playlistcontainer_loaded;

    plCallbacks.tracks_added = 0;
    plCallbacks.tracks_removed = 0;
    plCallbacks.tracks_moved = 0;
    plCallbacks.playlist_renamed = 0;
    plCallbacks.playlist_state_changed = 0;
    plCallbacks.playlist_update_in_progress = 0;
    plCallbacks.playlist_metadata_updated = 0;
    plCallbacks.track_created_changed = 0;
    plCallbacks.track_seen_changed = 0;
    plCallbacks.description_changed = 0;
    plCallbacks.image_changed = 0;
    plCallbacks.track_message_changed = 0;
    plCallbacks.subscribers_changed = 0;
    plCallbacks.playlist_state_changed = &playlist_state_changed;
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

sp_playlist_callbacks *Spotify_Wrapper::playlistCallbacks()
{
    if (s == 0) {
        return 0;
    }
    return &plCallbacks;
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

void Spotify_Wrapper::start_playback(sp_session *sp)
{
    s->startPlaybackCb(sp);
}

void Spotify_Wrapper::stop_playback(sp_session *sp)
{
    s->stopPlaybackCb(sp);
}

void Spotify_Wrapper::get_audio_buffer_stats(sp_session *sp, sp_audio_buffer_stats *stats)
{
    s->getAudioBufferStatsCb(sp, stats);
}

void Spotify_Wrapper::credentials_blob_updated(sp_session *sp, const char *data)
{
    s->credentialsBlobUpdated(sp, data);
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

void Spotify_Wrapper::playlist_state_changed(sp_playlist *pl, void *userdata)
{
    s->playlistStateChangedCb(pl, userdata);
}

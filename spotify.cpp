#include "spotify.h"
#include "spotify_ll.h"

#include <libspotify/api.h>
#include <QDebug>
#include <QString>
#include <QStringList>


void eq_put(void * obj, Event_t event)
{
    Spotify * _this = static_cast<Spotify*>(obj);
    _this->eq.put(event);
}


Spotify::Spotify(const QString &username, const QString &password) :
    user(username), pass(password), sp(0)
{
}

Spotify::~Spotify()
{
    if (sp) {
        sp_session_release(sp);
    }
}

void Spotify::run()
{
    int next_timeout = 0;

    sp = spotify_ll_init(this, eq_put);
    if (!sp) {
        return;
    }

    sp_session_login(sp,
                     user.toLocal8Bit().constData(),
                     pass.toLocal8Bit().constBegin(),
                     0, NULL);

    bool running = true;
    while (running) {
        Event_t ev;
        // Get next event (or timeout)
        if (next_timeout == 0) {
            eq.get(ev);
        } else {
            if (!eq.get(ev, next_timeout)) {
                // Timed out
                ev = EVENT_TIMEOUT;
            }
        }

        switch (ev) {
        case EVENT_SPOTIFY_MAIN_TICK:
        case EVENT_TIMEOUT:
            break;

        case EVENT_LOGGED_IN:
            spotify_ll_setup_playlistcontainer(sp);
            break;

        case EVENT_PLAYLIST_CONTAINER_LOADED:
            compileNewListOfPlaylists();
            logout();
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

void Spotify::logout()
{
    sp_error err = sp_session_logout(sp);
    if (err != SP_ERROR_OK) {
        qDebug() << "Unable to logout:" << QString::fromLocal8Bit(sp_error_message(err));
    } else {
        qDebug() << "Logout request posted";
    }
}

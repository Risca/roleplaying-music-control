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
    .callbacks = NULL,
    NULL,
};


sp_session *spotify_ll_init(sp_session_callbacks *session_callbacks)
{
    sp_error err;
    sp_session *sp;

    if (session_callbacks == NULL) {
        return NULL;
    }

    spconfig.callbacks = session_callbacks;
    spconfig.application_key_size = g_appkey_size;

    err = sp_session_create(&spconfig, &sp);
    if (err != SP_ERROR_OK) {
        fprintf(stderr, "Unable to create session: %s\n",
                sp_error_message(err));
        return NULL;
    }

    return sp;
}


#ifdef __cplusplus
}
#endif // __cplusplus

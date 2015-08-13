#include "sdlengine.h"

SDLEngine::SDLEngine() : initOk(false)
{
    QString audioDriverName = settings.value("SDL/AudioDriver").toString();
    if (!audioDriverName.isEmpty()) {
        QString envString = "SDL_AUDIODRIVER=" + audioDriverName;
        putenv(envString.toStdString().c_str());
    }

    if (SDL_Init(SDL_INIT_AUDIO) == 0) {
        initOk = true;
    }
}

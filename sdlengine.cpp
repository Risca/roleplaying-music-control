#include "sdlengine.h"

#if defined(Q_OS_WIN)
    #define SETENV(key, value) _putenv_s((key), (value))
#elif defined(Q_OS_UNIX)
    #define SETENV(key, value) setenv((key), (value), 1)
#endif

SDLEngine::SDLEngine() : initOk(false)
{
    QString audioDriverName = settings.value("SDL/AudioDriver").toString();
    if (!audioDriverName.isEmpty()) {
        SETENV("SDL_AUDIODRIVER", audioDriverName.toStdString().c_str());
    }

    if (SDL_Init(SDL_INIT_AUDIO) == 0) {
        initOk = true;
    }
}

bool SDLEngine::setAudioDriver(const QString &driverName)
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SETENV("SDL_AUDIODRIVER", driverName.toStdString().c_str());
    int rc = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (rc != 0) {
        fprintf(stderr, "Failed to initialize \"%s\": %s\n",
                driverName.toStdString().c_str(),
                SDL_GetError());
        return false;
    }
    return true;
}

QString SDLEngine::getCurrentAudioDriver()
{
    return QString(SDL_GetCurrentAudioDriver());
}

QStringList SDLEngine::getAudioDrivers()
{
    QStringList drivers;
    for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
        drivers << QString(SDL_GetAudioDriver(i));
    }
    return drivers;
}

QStringList SDLEngine::getAudioDevices()
{
    QStringList devices;
    const int count = SDL_GetNumAudioDevices(0);
    for (int i = 0; i < count; ++i) {
        devices += QString(SDL_GetAudioDeviceName(i, 0));
    }
    return devices;
}

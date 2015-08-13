#ifndef SDLWRAPPER_H
#define SDLWRAPPER_H

#include <QSettings>
#include <QStringList>
#include <SDL2/SDL.h>

class SDLEngine {
public:
    SDLEngine();
    ~SDLEngine() { if (initOk) SDL_Quit(); }
    bool initialized() { return initOk; }

    static bool setAudioDriver(const QString &driverName);
    static QString getCurrentAudioDriver();
    static QStringList getAudioDrivers();
    static QStringList getAudioDevices();

private:
    bool initOk;
    QSettings settings;

};

#endif // SDLWRAPPER_H

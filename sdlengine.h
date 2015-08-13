#ifndef SDLWRAPPER_H
#define SDLWRAPPER_H

#include <QSettings>
#include <SDL2/SDL.h>

class SDLEngine {
public:
    SDLEngine();
    ~SDLEngine() { if (initOk) SDL_Quit(); }
    bool initialized() { return initOk; }

private:
    bool initOk;
    QSettings settings;

};

#endif // SDLWRAPPER_H

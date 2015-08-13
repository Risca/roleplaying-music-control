#ifndef SDLWRAPPER_H
#define SDLWRAPPER_H

#include <SDL2/SDL.h>

class SDLEngine {
public:
    SDLEngine() : initOk(SDL_Init(SDL_INIT_AUDIO) == 0 ? true : false) {}
    ~SDLEngine() { if (initOk) SDL_Quit(); }
    bool initialized() { return initOk; }

private:
    bool initOk;
};

#endif // SDLWRAPPER_H


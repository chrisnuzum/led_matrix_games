#include "MyFuncs.h"

void delay(Uint32 delayTime)
{
    Uint32 beginTime = SDL_GetTicks();
    SDL_Event evt;
    bool _running = true;
    while (_running && SDL_GetTicks() < beginTime + delayTime)
    {
        SDL_Delay(1);
        SDL_PollEvent(&evt); // without polling for events, the window will show "Not responding" during long delays
        if (evt.type == SDL_QUIT)
            _running = false;
    }
    // SDL_Delay(delayTime);
}

Uint32 millis()
{
    return SDL_GetTicks();
}

long random(long howbig)
{
    if (howbig == 0)
    {
        return 0;
    }
    if (howbig < 0)
    {
        return (random(0, -howbig));
    }
    // if randomSeed was called, fall back to software PRNG
    uint32_t val = rand();
    return val % howbig;
}

long random(long howsmall, long howbig)
{
    if (howsmall >= howbig)
    {
        return howsmall;
    }
    long diff = howbig - howsmall;
    return random(diff) + howsmall;
}
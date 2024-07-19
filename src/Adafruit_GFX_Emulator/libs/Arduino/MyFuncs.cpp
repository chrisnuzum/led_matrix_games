#include "MyFuncs.h"

void delay(Uint32 ms)
{
    SDL_Delay(ms);
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
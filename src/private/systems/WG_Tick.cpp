#include "systems/WG_Tick.h"
#include "SDL.h"
#include "spdlog/spdlog.h"

void WG_Tick::Tick()
{
    Uint32 currentTime = SDL_GetTicks();
    OnTick.Emit(static_cast<float>(currentTime - LastTickTime));
    LastTickTime = currentTime;
}
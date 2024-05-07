#pragma once

#include "SDL.h"
#include <chrono>
#include "WG_EventEmitter.h"
#include "WG_System.h"


class WG_Tick: public WG_System {
public:
    WG_EventEmitter<float> OnTick;
    Uint32 LastTickTime = 0;
    void Tick() override;
};

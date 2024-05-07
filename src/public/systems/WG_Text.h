#pragma once

#include "SDL_ttf.h"
#include "WG_System.h"

class WG_Text : public WG_System {
public:
    TTF_Font *Font;
    SDL_Surface *Surface;
    SDL_Texture *Texture;

    void Start() override;

    void Tick() override;

    void Cleanup() override;
};

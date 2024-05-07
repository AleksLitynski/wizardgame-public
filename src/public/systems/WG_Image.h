#pragma once

#include "WG_System.h"

class WG_Image : public WG_System {
public:
    SDL_Rect SquareRect;
    SDL_Rect ImgRect;
    SDL_Texture *Texture;


    void Start() override;

    void Tick() override;

    void Cleanup() override;
};

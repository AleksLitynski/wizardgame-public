
#include "systems/WG_Text.h"


#include "SDL_image.h"
#include "SDL_ttf.h"

#include <stdexcept>
#include <filesystem>

using namespace std;

void WG_Text::Start() {
    TTF_Init();

    Font = TTF_OpenFont((filesystem::current_path().string() + "/assets/OpenSans-Regular.ttf").c_str(), 25);
    SDL_Color black = {0, 0, 0};
    Surface = TTF_RenderText_Solid(Font, "Some dummy text?", black);
}


void WG_Text::Tick() {
    SDL_Rect text_rect;
    text_rect.x = 0;
    text_rect.y = 0;
    text_rect.w = 100;
    text_rect.h = 100;
//    SDL_RenderCopy(Engine->Renderer, Texture, NULL, &text_rect);
}

void WG_Text::Cleanup() {
    SDL_FreeSurface(Surface);
    SDL_DestroyTexture(Texture);
    TTF_CloseFont(Font);
    TTF_Quit();
}


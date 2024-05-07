#include <cstdio>
#include "systems/WG_Image.h"
#include "SDL_image.h"
#include "systems/WG_OpenGL.h"
#include "spdlog/spdlog.h"

#include <stdexcept>
#include <filesystem>

#include "WG_Engine.h"

using namespace std;

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


void WG_Image::Start() {
    std::shared_ptr<WG_OpenGL> openGl = GetEngine()->Systems.Ensure<WG_OpenGL>();

    // Initialize SDL2_image
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        spdlog::error("SDL2_image could not be initialized. Error: {}", IMG_GetError());
        throw runtime_error("SDL2_image failed");
    }

    // Load image from file
//    Texture = IMG_LoadTexture(Engine->Renderer, (filesystem::current_path().string() + "/assets/wizard.jpg").c_str());
    if (!Texture) {
        printf("Unable to load image. Error: %s\n", IMG_GetError());
        throw runtime_error("SDL2_image failed");
    }

    // Get dimensions
    int TexWidth, TexHeight;
    SDL_QueryTexture(Texture, nullptr, nullptr, &TexWidth, &TexHeight);

    int WinWidth, WinHeight;
    SDL_GetWindowSize(GetEngine()->Window, &WinWidth, &WinHeight);
    if (TexWidth > TexHeight) {
        ImgRect.w = WinWidth * 0.6;
        ImgRect.h = TexHeight * ImgRect.w / TexWidth;
    } else {
        ImgRect.h = WinHeight * 0.6;
        ImgRect.w = TexWidth * ImgRect.h / TexHeight;
    }

    ImgRect.x = WinWidth / 2 - ImgRect.w / 2;
    ImgRect.y = WinHeight / 2 - ImgRect.h / 2;

    // Set the border size
    const int BorderSize = 5;

    // Square dimensions
    SquareRect.w = ImgRect.w + BorderSize * 2;
    SquareRect.h = ImgRect.h + BorderSize * 2;

    // Square position:
    SquareRect.x = ImgRect.x - BorderSize;
    SquareRect.y = ImgRect.y - BorderSize;
}

void WG_Image::Tick() {
//    SDL_RenderFillRect(Engine->Renderer, &SquareRect);
//    SDL_RenderCopy(Engine->Renderer, Texture, NULL, &ImgRect);
}

void WG_Image::Cleanup() {
    IMG_Quit();
}
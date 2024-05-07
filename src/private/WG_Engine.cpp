#include "WG_Engine.h"

#include "WG_System.h"
#include "WG_EntityManager.h"
#include <cstdio>
#include <stdexcept>

using namespace std;


WG_Engine::WG_Engine()
{
    Systems.Provision = [this](std::string& id, const std::shared_ptr<WG_System>& instance)
    {
        instance->Engine = weak_from_this();
        instance->SystemId = id;
        instance->Start();
    };
    Systems.Deprovision = [this](const std::shared_ptr<WG_System>& instance)
    {
        instance->Cleanup();
    };
}

void WG_Engine::Start() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        throw runtime_error("SDL init failed");
    }

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    Window = SDL_CreateWindow(
            "Wizard Game",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 600,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_StartTextInput();
}

void WG_Engine::Run() {
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
                break;
            }
            for (std::shared_ptr<WG_System> system: Systems) {
                system->OnEvent(event);
            }
        }

        // let systems tick
        for (auto system: Systems) {
            system->Tick();
        }
    }
}

void WG_Engine::Cleanup() {
    std::vector<std::shared_ptr<WG_System>> SystemsVec;
    for (std::shared_ptr<WG_System> system: Systems) SystemsVec.push_back(system);
    for (const std::shared_ptr<WG_System>& weakSys: SystemsVec) {
        Systems.Remove(typeid(weakSys.get()));
    }

    SDL_StopTextInput();
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

std::shared_ptr<WG_EntityManager> WG_Engine::GetEntityManager() {
    return Systems.Ensure<WG_EntityManager>();
}
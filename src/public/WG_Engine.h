#pragma once

#include <map>
#include <memory>
#include <concepts>
#include <vector>
#include <optional>

#include "SDL.h"
#include "WG_InstancesSet.h"

class WG_System;
class WG_EntityManager;

class WG_Engine : public std::enable_shared_from_this<WG_Engine>  {
public:
    SDL_Window *Window;
    WG_InstancesSet<WG_System> Systems;

    // This is slightly sloppy. The engine doesn't necessarily have an EntityManager system, but it's so common that it's helpful to have a way to just grab the entity manager directly
    std::shared_ptr<WG_EntityManager> GetEntityManager();

    WG_Engine();

    void Start();
    void Run();
    void Cleanup();
};

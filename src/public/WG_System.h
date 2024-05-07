#pragma once

#include <memory>
#include <string>
#include "SDL.h"

class WG_Engine;

class WG_System {
public:
    std::weak_ptr<WG_Engine> Engine;
    std::shared_ptr<WG_Engine> GetEngine() const;
    std::string SystemId;

    // Start will be called after the 'Engine' variable is set
    // In the future, systems may have other variables that need to be set before they 'Start'
    virtual void Start() {}
    virtual void Tick() {}
    virtual void Cleanup() {}
    virtual void OnEvent(SDL_Event &event) {}
};

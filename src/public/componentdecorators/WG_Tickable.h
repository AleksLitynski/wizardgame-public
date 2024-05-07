#pragma once

#include <functional>
#include "WG_ComponentDecorator.h"
#include "SDL.h"
#include "systems/WG_Tick.h"
#include "spdlog/spdlog.h"

class WG_Tickable: public virtual WG_ComponentDecorator {
public:
    WG_Tickable()
    {
        int tickSubId = 0;
        StartDecorators.emplace_back([this, &tickSubId](){
             // std::bind(&WG_Tickable::Tick, this, std::placeholders::_1));
             // ^ weirdly enough, bind is slower than a wrapper function, because it's held as an object and the wrapper is handled by the compiler, I guess?
            std::shared_ptr<WG_Tick> tickSys = GetEngine()->Systems.Ensure<WG_Tick>();
            tickSubId = tickSys->OnTick.Subscribe([this](float delta){
                Tick(delta);
            });
        });

        CleanupDecorators.emplace_back([this, &tickSubId](){
            GetEngine()->Systems.Ensure<WG_Tick>()->OnTick.UnSubscribe(tickSubId);
        });
    }

    virtual void Tick(float delta) = 0;
};

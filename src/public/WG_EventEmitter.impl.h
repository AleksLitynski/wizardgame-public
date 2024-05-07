#pragma once
#include "WG_EventEmitter.impl.h"

#include <map>
#include <vector>
#include <functional>

template<typename... EventArgs>
int WG_EventEmitter<EventArgs...>::Subscribe(std::function<void(EventArgs...)> newHandler)
{
    Handlers[HandlerIdx] = newHandler;
    return HandlerIdx++;
}

template<typename... EventArgs>
void WG_EventEmitter<EventArgs...>::UnSubscribe(int handlerId)
{
    Handlers.erase(handlerId);
}

template<typename... EventArgs>
void WG_EventEmitter<EventArgs...>::Emit(EventArgs... Args)
{
    for(auto handler : Handlers) {
        handler.second(std::forward<EventArgs>(Args)...);
    }
}
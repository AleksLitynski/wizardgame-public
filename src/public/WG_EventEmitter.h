#pragma once

#include <map>
#include <vector>
#include <functional>

template<typename... EventArgs>
class WG_EventEmitter {
public:
    int Subscribe(std::function<void(EventArgs...)> newHandler);
    void UnSubscribe(int handlerId);
    void Emit(EventArgs... Args);
private:
    std::map<int, std::function<void(EventArgs...)>> Handlers;
    int HandlerIdx = 0;
};

#include "WG_EventEmitter.impl.h"

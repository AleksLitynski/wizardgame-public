#pragma once
#include <vector>
#include <functional>

#include "WG_Component.h"

class WG_ComponentDecorator : public virtual WG_Component {
protected:
    std::vector<std::function<void()>> StartDecorators;
    std::vector<std::function<void()>> CleanupDecorators;

public:
    void DoStartDecorators();
    void DoCleanupDecorator();
};

#include "WG_ComponentDecorator.h"


void WG_ComponentDecorator::DoStartDecorators()
{
    for(const auto& start : StartDecorators)
    {
        start();
    }
}

void WG_ComponentDecorator::DoCleanupDecorator()
{
    for(const auto& cleanup : CleanupDecorators)
    {
        cleanup();
    }
}
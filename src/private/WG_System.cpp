#include "WG_System.h"


std::shared_ptr<WG_Engine> WG_System::GetEngine() const
{
    return Engine.lock();
}
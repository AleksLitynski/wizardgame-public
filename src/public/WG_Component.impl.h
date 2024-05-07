#pragma once
#include "WG_Component.h"

#include <memory>
#include <string>
#include "WG_Engine.h"
#include "WG_Entity.h"


template<std::derived_from<WG_Component> InstanceType>
std::shared_ptr<InstanceType> WG_Component::EnsureComponent()
{
     return GetEntity()->EnsureComponent<InstanceType>();
}


template<std::derived_from<WG_System> SystemType>
std::shared_ptr<SystemType> WG_Component::EnsureSystem()
{
    return GetEngine()->Systems.Ensure<SystemType>();
}

template<typename... ComponentTypes> std::shared_ptr<WG_Entity> WG_Component::MakeEntity()
{
    return GetEntity()->GetEntityManager()->MakeEntity<ComponentTypes...>();
}
template<typename... ComponentTypes> std::shared_ptr<WG_Entity> WG_Component::MakeEntity(std::optional<std::string> name)
{
    return GetEntity()->GetEntityManager()->MakeEntity<ComponentTypes...>(name);
}
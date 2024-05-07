#pragma once
#include "WG_Entity.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SDL.h"
#include "WG_Engine.h"
//#include "WG_Component.h"
#include "WG_EntityManager.h"

template<std::derived_from<WG_Component> ComponentType>
std::shared_ptr<WG_Entity> WG_Entity::WithComponent()
{
    Components.Ensure<ComponentType>();
    return shared_from_this();
}

template<std::derived_from<WG_Component> InstanceType, typename ...InstanceConstructorArgs>
std::shared_ptr<InstanceType> WG_Entity::EnsureComponent(InstanceConstructorArgs && ...args)
{
    return Components.Ensure<InstanceType>(std::forward(args)...);
}


template<typename NextInstanceType, typename... InstanceTypes>
void WG_Entity::EnsureComponents()
{
    if constexpr (!std::same_as<void, NextInstanceType>)
    {
        EnsureComponent<NextInstanceType>();
        EnsureComponents<InstanceTypes..., void>();
    }
}


template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_Entity::MakeEntity(std::optional<std::string> name)
{
    return GetEntityManager()->MakeEntity<ComponentTypes...>(name);
}

template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_Entity::MakeEntity()
{
    return GetEntityManager()->MakeEntity<ComponentTypes...>();
}


template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_Entity::MakeChild()
{
    return MakeChild<ComponentTypes...>(std::nullopt);
}

template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_Entity::MakeChild(std::optional<std::string> name)
{
    std::shared_ptr<WG_Entity> entity = MakeEntity<ComponentTypes...>(name);
    entity->SetParent(shared_from_this());
    return entity;

}
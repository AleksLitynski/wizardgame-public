#pragma once
#include "WG_EntityManager.impl.h"

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <format>
#include "WG_Component.h"
#include "WG_System.h"
#include "WG_Entity.h"

template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_EntityManager::MakeEntity(std::optional<std::string> name)
{
    std::shared_ptr<WG_Entity> Entity = std::make_shared<WG_Entity>(); // make the entity
    Entity->EntityId = EntityIdIdx++; // set child ID
    Entity->EntityManager = weak_from_this(); // set the EntityManager
    Entities[Entity->EntityId] = Entity;
    Entity->Name = name.has_value() ? name.value() : std::format("Entity {}", Entity->EntityId);
    Entity->EnsureComponents<ComponentTypes..., void>();
    return Entity;
}

template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_EntityManager::MakeEntity()
{
    return MakeEntity<ComponentTypes...>(std::nullopt);
}


template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_EntityManager::MakeChild()
{
    return RootEntity->MakeChild<ComponentTypes...>();
}

template<typename... ComponentTypes>
std::shared_ptr<WG_Entity> WG_EntityManager::MakeChild(std::optional<std::string> name)
{
    return RootEntity->MakeChild<ComponentTypes...>(name);
}


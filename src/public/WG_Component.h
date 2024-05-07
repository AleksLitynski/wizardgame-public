#pragma once

#include <memory>
#include <string>
#include <optional>
#include <map>
#include "WG_PropsMap.h"

class WG_Entity;
class WG_Engine;
class WG_System;
class WG_EntityManager;

class WG_Component : public std::enable_shared_from_this<WG_Component> {

public:
    std::weak_ptr<WG_Entity> Entity;
    std::shared_ptr<WG_Entity> GetEntity() const;
    std::string ComponentId;
    int GetEntityId() const;
    virtual void Start() {}
    virtual void Cleanup() {}
    virtual void SetProps(WG_PropsMap props) {}
    std::shared_ptr<WG_EntityManager> GetEntityManager() const;
    std::shared_ptr<WG_Engine> GetEngine() const;

    template<std::derived_from<WG_System> SystemType>
    std::shared_ptr<SystemType> EnsureSystem();

    template<std::derived_from<WG_Component> InstanceType>
    std::shared_ptr<InstanceType> EnsureComponent();

    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity();
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity(std::optional<std::string> name);
};

#include "WG_Component.impl.h"
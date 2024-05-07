#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SDL.h"
#include "WG_InstancesSet.h"
#include "WG_PropsMap.h"


class WG_Engine;
class WG_EntityManager;
class WG_Component;
class WG_Transform;


// final because entities are really just meant to be a collection of components. They're meant to be extended by adding
// components
class WG_Entity final : public std::enable_shared_from_this<WG_Entity> {
public:
    WG_Entity();
    WG_InstancesSet<WG_Component> Components;
    template<std::derived_from<WG_Component> InstanceType, typename ...InstanceConstructorArgs> std::shared_ptr<InstanceType> EnsureComponent(InstanceConstructorArgs && ...args);

    std::shared_ptr<WG_Entity> SetProps(std::map<std::string, std::string> props);

    template<std::derived_from<WG_Component> ComponentType>
    std::shared_ptr<WG_Entity> WithComponent();

    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity();
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity(std::optional<std::string> name);

    int EntityId;
    std::string Name; // not populated automatically. Feel free to do whatever with the name
    std::weak_ptr<WG_EntityManager> EntityManager;
    std::shared_ptr<WG_EntityManager> GetEntityManager() const;


    template<typename NextInstanceType, typename... InstanceType> void EnsureComponents();

    // Kind of hacky to have a direct reference to a specific component in WG_Entity, but I expect the transform component is uniform enough, it'll be useful to have the reference
    // Plus, unity does it, so why shouldn't I?
    std::shared_ptr<WG_Transform> GetTransform();
    void SetParent(const std::shared_ptr<WG_Transform>& newParent);
    void SetParent(const std::shared_ptr<WG_Entity>& newParent);
    std::shared_ptr<WG_Entity> WithChildren(const std::vector<std::shared_ptr<WG_Transform>>& newChildren);
    std::shared_ptr<WG_Entity> WithChildren(const std::vector<std::shared_ptr<WG_Entity>>& newChildren);

    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeChild();
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeChild(std::optional<std::string> name);

};


#include "WG_Entity.impl.h"
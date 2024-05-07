#pragma once
#include "WG_InstancesSet.h"

#include <concepts>
#include <map>
#include <optional>
#include <memory>
#include <algorithm>
#include <vector>
#include <stdexcept>

#include "WG_EventEmitter.h"

// A set of instances which are all derived from "BaseType".
// Each derived type can only be added to the set once.
// Derived instances can be retrieved by their type

template<typename BaseType>
template<std::derived_from<BaseType> InstanceType, typename ...InstanceConstructorArgs>
std::shared_ptr<InstanceType> WG_InstancesSet<BaseType>::Add(InstanceConstructorArgs && ...args) {
    std::string typeId = typeid(InstanceType).name();
    if (Instances.contains(typeId)) {
        throw std::runtime_error(typeId + " instance already added. Use 'Ensure' instead to upsert instead of create an instance.");
    }

    std::shared_ptr<InstanceType> instance = std::make_shared<InstanceType>(std::forward<InstanceConstructorArgs>(args)...);
    Instances.insert({typeId, (std::shared_ptr<BaseType>)instance});
    Provision(typeId, instance);
    OnAdd.Emit(instance);
    return instance;
}

template<typename BaseType>
template<std::derived_from<BaseType> InstanceType>
void WG_InstancesSet<BaseType>::Remove() {
    return Remove(typeid(InstanceType));
}

template<typename BaseType>
void WG_InstancesSet<BaseType>::Remove(const std::type_info& toRemove)
{
    if (!Instances.contains(toRemove.name())) return;
    std::shared_ptr<BaseType> instance = Instances[toRemove.name()];
    Deprovision(instance);
    OnRemove.Emit(instance);
    Instances.erase(toRemove.name());
}

template<typename BaseType>
template<std::derived_from<BaseType> InstanceType>
std::optional<std::shared_ptr<InstanceType>> WG_InstancesSet<BaseType>::Get() {
    std::string typeId = typeid(InstanceType).name();
    if (Instances.count(typeId) == 0) {
        return std::nullopt;
    }
    return std::dynamic_pointer_cast<InstanceType>(Instances[typeId]);
}

template<typename BaseType>
template<std::derived_from<BaseType> InstanceType, typename ...InstanceConstructorArgs>
std::shared_ptr<InstanceType> WG_InstancesSet<BaseType>::Ensure(InstanceConstructorArgs && ...args) {
    std::optional<std::shared_ptr<InstanceType>> current = Get<InstanceType>();
    return current.has_value()
        ? current.value()
        : Add<InstanceType>(std::forward<InstanceConstructorArgs>(args)...);
}

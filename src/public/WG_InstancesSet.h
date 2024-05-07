#pragma once

#include <concepts>
#include <map>
#include <optional>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include "WG_EventEmitter.h"

// A set of instances which are all derived from "BaseType".
// Each derived type can only be added to the set once.
// Derived instances can be retrieved by their type

template<typename BaseType>
class WG_InstancesSet {
public:
    WG_EventEmitter<std::shared_ptr<BaseType>> OnAdd;
    WG_EventEmitter<std::shared_ptr<BaseType>> OnRemove;

    template<std::derived_from<BaseType> InstanceType, typename ...InstanceConstructorArgs> std::shared_ptr<InstanceType> Add(InstanceConstructorArgs && ...args);

    template<std::derived_from<BaseType> InstanceType, typename ...InstanceConstructorArgs> std::shared_ptr<InstanceType> Ensure(InstanceConstructorArgs && ...args);

    template<std::derived_from<BaseType> InstanceType> void Remove();
    void Remove(const std::type_info& toRemove);

    template<std::derived_from<BaseType> InstanceType> std::optional<std::shared_ptr<InstanceType>> Get();

    std::function<void(std::string& TypeId, std::shared_ptr<BaseType>)> Provision;
    std::function<void(std::shared_ptr<BaseType>)> Deprovision;

    // inherit most of our iterator functionality from std::map, but just return the value, not the key
    // iterators are structurally defined, so this doesn't have to implement a specific base class,
    // just has to have methods a template will try to call, per https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
    struct Iterator {
        explicit Iterator(std::map<std::string, std::shared_ptr<BaseType>>::iterator inner): Inner(inner) {}
        std::shared_ptr<BaseType> operator*() const { return std::shared_ptr<BaseType>(Inner->second); }
        Iterator& operator++() { ++Inner; return *this; }
        bool operator!=(const Iterator& other) const { return Inner != other.Inner; }

    private:
        std::map<std::string, std::shared_ptr<BaseType>>::iterator Inner;
    };
    Iterator begin() {
        return Iterator{Instances.begin()};
    }
    Iterator end() {
        return Iterator{Instances.end()};
    }

protected:
    std::map<std::string, std::shared_ptr<BaseType>> Instances;

};

#include "WG_InstancesSet.impl.h"

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <format>
#include "WG_System.h"

class WG_Component;
class WG_Entity;

class WG_EntityManager : public std::enable_shared_from_this<WG_EntityManager>, public WG_System {
public:
    // TODO: Use concepts to constrain typenames to always extend WG_Component or be void
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity(std::optional<std::string> name);
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeEntity();

    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeChild();
    template<typename... ComponentTypes> std::shared_ptr<WG_Entity> MakeChild(std::optional<std::string> name);


    void RemoveEntity(int toRemoveId);
    void RemoveEntity(const std::shared_ptr<WG_Entity>& toRemove);
    std::shared_ptr<WG_Entity> RootEntity;

    void Start() override;

    void Debug_LogEntityTree();


    struct Iterator {
        explicit Iterator(std::map<int, std::shared_ptr<WG_Entity>>::iterator inner): Inner(inner) {}
        std::shared_ptr<WG_Entity> operator*() const { return Inner->second; }
        Iterator& operator++() { Inner++; return *this; }
        bool operator==(Iterator other) const;
        bool operator!=(Iterator other) const;

    private:
        std::map<int, std::shared_ptr<WG_Entity>>::iterator Inner;
    };
    Iterator begin() {
        return Iterator{Entities.begin()};
    }
    Iterator end() {
        return Iterator{Entities.end()};
    }

private:
    int EntityIdIdx = 0;
    std::map<int, std::shared_ptr<WG_Entity>> Entities;
};


#include "WG_EntityManager.impl.h"

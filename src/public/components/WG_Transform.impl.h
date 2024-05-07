#pragma once
#include "WG_Transform.h"
#include "WG_Entity.h"


template<typename ComponentType>
void WG_Transform::TraverseComponent(const std::function<void(std::shared_ptr<ComponentType>)>& OnItem)
{
    TraverseComponent<ComponentType>(OnItem, [](std::shared_ptr<ComponentType> _){});
}

template<typename ComponentType>
void WG_Transform::TraverseComponent(const std::function<void(std::shared_ptr<ComponentType>)>& OnItemPre, const std::function<void(std::shared_ptr<ComponentType>)>& OnItemPost)
{
    Traverse([&OnItemPre](const std::shared_ptr<WG_Transform>& item) {
        std::optional<std::shared_ptr<ComponentType>> comp = item->GetEntity()->Components.Get<ComponentType>();
        if (comp.has_value()) OnItemPre(comp.value());
    }, [&OnItemPost](const std::shared_ptr<WG_Transform>& item) {
        std::optional<std::shared_ptr<ComponentType>> comp = item->GetEntity()->Components.Get<ComponentType>();
        if (comp.has_value()) OnItemPost(comp.value());
    });
}
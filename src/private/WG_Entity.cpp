#include "WG_Entity.h"

#include <string>
#include <algorithm>
#include <cctype>
#include "WG_Component.h"
#include "WG_ComponentDecorator.h"
#include "WG_EntityManager.h"
#include "components/WG_Transform.h"
#include "WG_StringUtils.h"

WG_Entity::WG_Entity()
{
    Components.Provision = [this](std::string& id, const std::shared_ptr<WG_Component>& instance)
    {
        instance->Entity = weak_from_this();
        instance->ComponentId = id;
        std::shared_ptr<WG_ComponentDecorator> decorator = std::dynamic_pointer_cast<WG_ComponentDecorator>(std::shared_ptr<WG_Component>(instance));
        if (decorator != nullptr)
        {
            decorator->DoStartDecorators();
        }
        instance->Start();
    };
    Components.Deprovision = [this](const std::shared_ptr<WG_Component>& instance)
    {
        std::shared_ptr<WG_ComponentDecorator> decorator = std::dynamic_pointer_cast<WG_ComponentDecorator>(std::shared_ptr<WG_Component>(instance));
        if (decorator != nullptr)
        {
            decorator->DoCleanupDecorator();
        }
        instance->Cleanup();
    };
}


std::shared_ptr<WG_EntityManager> WG_Entity::GetEntityManager() const
{
    return EntityManager.lock();
}

std::shared_ptr<WG_Transform> WG_Entity::GetTransform()
{
    return EnsureComponent<WG_Transform>();
}

void WG_Entity::SetParent(const std::shared_ptr<WG_Transform>& newParent)
{
    return GetTransform()->SetParent(newParent);
}
void WG_Entity::SetParent(const std::shared_ptr<WG_Entity>& newParent)
{
    GetTransform()->SetParent(newParent);
}

std::shared_ptr<WG_Entity> WG_Entity::WithChildren(const std::vector<std::shared_ptr<WG_Transform>>& newChildren)
{
    auto thisShared = shared_from_this();
    for (auto c : newChildren) c->SetParent(thisShared);
    return thisShared;
}
std::shared_ptr<WG_Entity> WG_Entity::WithChildren(const std::vector<std::shared_ptr<WG_Entity>>& newChildren)
{
    auto thisShared = shared_from_this();
    for (auto c : newChildren) c->SetParent(thisShared);
    return thisShared;
}

std::shared_ptr<WG_Entity> WG_Entity::SetProps(std::map<std::string, std::string> props)
{
    WG_PropsMap Props(props);
    Props.WithString("name", [this](auto s) {
        Name = s;
    });
    for (auto comp : Components) comp->SetProps(props);
    return shared_from_this();
}
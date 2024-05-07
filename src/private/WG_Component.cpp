#include "WG_Component.h"
#include "WG_Entity.h"
#include "WG_EntityManager.h"

std::shared_ptr<WG_Engine> WG_Component::GetEngine() const
{
    return GetEntityManager()->GetEngine();
}

std::shared_ptr<WG_EntityManager> WG_Component::GetEntityManager() const
{
    return GetEntity()->GetEntityManager();
}

std::shared_ptr<WG_Entity> WG_Component::GetEntity() const
{
    return Entity.lock();
}

int WG_Component::GetEntityId() const
{
    return GetEntity()->EntityId;
}
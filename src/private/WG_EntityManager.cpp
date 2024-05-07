#include "WG_EntityManager.h"
#include "components/WG_Transform.h"
#include "spdlog/spdlog.h"

void WG_EntityManager::Start()
{
    RootEntity = MakeEntity<WG_Transform>("Root");
}

void WG_EntityManager::RemoveEntity(const std::shared_ptr<WG_Entity>& toRemove)
{
    Entities.erase(toRemove->EntityId);
}

void WG_EntityManager::RemoveEntity(int toRemoveId)
{
    Entities.erase(toRemoveId);
}

// This may need to be re-added depending on the exact #include order. Breaking dependency cycles in header files with templates is hell

bool WG_EntityManager::Iterator::operator==(WG_EntityManager::Iterator other) const {
    return Inner->first == (*other)->EntityId;
}

bool WG_EntityManager::Iterator::operator!=(WG_EntityManager::Iterator other) const {
    return Inner->first != (*other)->EntityId;
}

void WG_EntityManager::Debug_LogEntityTree()
{
    std::string outputStr;
    std::function<void(std::shared_ptr<WG_Entity>, int)> ConcatEntity = [&](const std::shared_ptr<WG_Entity>& entity, int depth) {

        std::string componentNames;
        for (std::shared_ptr<WG_Component> comp: entity->Components) {
            componentNames += (componentNames.empty() ? "" : ", ") + comp->ComponentId;
        }

        outputStr += std::format("{}{}|{} [{}]", std::string(depth * 4 + (depth == 0 ? 0 : 4), ' '), entity->EntityId, entity->Name, componentNames);
        auto Transform = entity->Components.Get<WG_Transform>();
        if (Transform.has_value()) {
            outputStr += std::format(" <{}>\n", Transform.value()->Print());
            for (const std::weak_ptr<WG_Transform>& child: Transform.value()->Children)
            {
                ConcatEntity(child.lock()->GetEntity(), depth + 1);
            }
        }
        else
        {
            outputStr += "\n";
        }
    };

    outputStr += "===== Scene Graph =====\n -> ";
    ConcatEntity(RootEntity, 0);
    for (const auto& entity: Entities)
    {
        if (entity.second->EntityId == RootEntity->EntityId) continue;
        auto transform = entity.second->Components.Get<WG_Transform>();
        if (transform.has_value() && transform->get()->Root.lock()->GetEntityId() != entity.second->EntityId) continue;
        outputStr += " -> ";
        ConcatEntity(entity.second, 0);
    }
    outputStr += "=======================\n";
    spdlog::info("\n{}", outputStr);
}

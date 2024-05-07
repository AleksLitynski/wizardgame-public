#include <utility>

#include "components/WG_Transform.h"

#include "WG_Entity.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "spdlog/spdlog.h"
#include "WG_Macros.h"
#include "WG_Entity.h"

void WG_Transform::Start()
{
    // Edge case. If the RootEntity is still being constructed, we become our own root. Otherwise, the root entity is the root
    Root = GetEntityManager()->RootEntity
        ? GetEntityManager()->RootEntity->EnsureComponent<WG_Transform>()
        : dynamic_pointer_cast<WG_Transform>(shared_from_this());

    Parent = Root;
}


void WG_Transform::SetParent(const std::shared_ptr<WG_Transform>& newParent)
{
    // remove from our current parent
    std::erase_if(Parent.lock()->Children, [this](const std::weak_ptr<WG_Transform>& other){
        return GetEntity()->EntityId == other.lock()->GetEntity()->EntityId;
    });
    Parent = newParent; // set our parent
    newParent->Children.push_back(std::static_pointer_cast<WG_Transform>(shared_from_this())); // add us to our new parents children
    Root = newParent->Root; // set our root to our new root
    OnParentSet.Emit(Parent.lock());
}


bool WG_Transform::IsRoot() {
    return GetEntityId() == GetEntityManager()->RootEntity->EntityId;
}

int WG_Transform::Depth()
{
    if (IsRoot()) return 0;
    return 1 + Parent.lock()->Depth();
}

void WG_Transform::SetParent(const std::shared_ptr<WG_Entity>& newParent)
{
    SetParent(newParent->GetTransform());
}


std::shared_ptr<WG_Transform> WG_Transform::GetRoot() const
{
    return Root.lock();
}
std::shared_ptr<WG_Transform> WG_Transform::GetParent() const
{
    return Parent.lock();
}

glm::mat4 WG_Transform::GetLocalMatrix() const
{
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, Position);
    matrix = glm::scale(matrix, Scale);
    matrix = matrix * glm::toMat4(Rotation);
    return matrix;
}

glm::mat4 WG_Transform::GetWorldMatrix()
{
    return IsRoot()
        ? GetLocalMatrix()
        : GetParent()->GetWorldMatrix() * GetLocalMatrix();
}


std::string WG_Transform::Print()
{
    return std::format("pos: {} | rot: {} | scale: {}",
        glm::to_string(Position),
        glm::to_string(Rotation),
        glm::to_string(Scale)
    );
}


void WG_Transform::Traverse(const std::function<void(std::shared_ptr<WG_Transform>)>& OnItem)
{
    Traverse(OnItem, [](const std::shared_ptr<WG_Transform>& _){});
}


void WG_Transform::Traverse(const std::function<void(std::shared_ptr<WG_Transform>)>& OnItemPre, const std::function<void(std::shared_ptr<WG_Transform>)>& OnItemPost)
{
    OnItemPre(GetEntity()->EnsureComponent<WG_Transform>());
    for (const auto& child : Children) child.lock()->Traverse(OnItemPre, OnItemPost);
    OnItemPost(GetEntity()->EnsureComponent<WG_Transform>());
}
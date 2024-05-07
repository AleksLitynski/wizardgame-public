#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <vector>
#include "WG_Component.h"


class WG_Transform: public WG_Component {

public:
    void Start() override;

    glm::vec3 Position = glm::vec3(0.0f);
    glm::quat Rotation = glm::identity<glm::quat>();
    glm::vec3 Scale = glm::vec3(1.0f);

    void SetProps(WG_PropsMap props) override
    {
        props.WithVec3("position", [this](auto s) {
            Position = s;
        });
        props.WithVec3("rotation", [this](auto s) {
            Rotation = s;
        });
        props.WithVec3("scale", [this](auto s) {
            Scale = s;
        });
    }

    glm::mat4 GetLocalMatrix() const;
    glm::mat4 GetWorldMatrix();

    std::weak_ptr<WG_Transform> Root;
    std::weak_ptr<WG_Transform> Parent;
    std::shared_ptr<WG_Transform> GetRoot() const;
    std::shared_ptr<WG_Transform> GetParent() const;
    std::vector<std::weak_ptr<WG_Transform>> Children;

    bool IsRoot();
    int Depth();

    void SetParent(const std::shared_ptr<WG_Transform>& newParent);
    void SetParent(const std::shared_ptr<WG_Entity>& newParent);

    WG_EventEmitter<std::shared_ptr<WG_Transform>> OnParentSet;

    std::string Print();

    void Traverse(const std::function<void(std::shared_ptr<WG_Transform>)>& OnItem);
    void Traverse(const std::function<void(std::shared_ptr<WG_Transform>)>& OnItemPre, const std::function<void(std::shared_ptr<WG_Transform>)>& OnItemPost);
    template<typename ComponentType>
    void TraverseComponent(const std::function<void(std::shared_ptr<ComponentType>)>& OnItem);
    template<typename ComponentType>
    void TraverseComponent(const std::function<void(std::shared_ptr<ComponentType>)>& OnItemPre, const std::function<void(std::shared_ptr<ComponentType>)>& OnItemPost);
};

#include "WG_Transform.impl.h"
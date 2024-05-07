#pragma once

#include "SDL.h"
#include "systems/WG_OpenGL.h"
#include "componentdecorators/WG_Tickable.h"
#include "components/WG_Transform.h"
#include <memory>
#include <vector>
#include "tiny_gltf.h"

class WG_Component;

class WG_GltfLoader: public WG_Component {

public:
    std::weak_ptr<WG_OpenGL> OpenGL;
    void LoadGltf(const std::string& path);
    void SetProps(WG_PropsMap props) override
    {
        props.WithString("gltf-mesh", [this](auto s) {
            LoadGltf(s);
        });
    }

private:
    std::weak_ptr<WG_Entity> RootNode;
    static std::shared_ptr<tinygltf::Model> LoadGltfFile(const std::string& path); // helper that loads a .gltf file into a tinygltf::Model. Use LoadGltf to get a WG_Entity back

    void LoadNode(const std::shared_ptr<WG_Entity>& parent, const std::shared_ptr<tinygltf::Model>& model, tinygltf::Node& node);
    void LoadPrimitive(const std::shared_ptr<WG_Entity>& self, const std::shared_ptr<tinygltf::Model>& model, tinygltf::Primitive& primitive);

    // TODO: Make this support all accessor types, not just the hard-coded ones
    template<int componentType, int componentSize> void VerifyAccessor(tinygltf::Accessor& accessor);
    std::vector<glm::vec3> LoadAccessor_Vec3_Float(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor);
    std::vector<glm::vec2> LoadAccessor_Vec2_Float(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor);
    std::vector<GLuint> LoadAccessor_Scalar_UnsignedShort(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor);
    std::vector<unsigned char> LoadBufferView(const std::shared_ptr<tinygltf::Model>& model, tinygltf::BufferView& bufferView);

};


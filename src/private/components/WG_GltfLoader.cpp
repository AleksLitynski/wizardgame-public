#include "components/WG_GltfLoader.h"
#include "WG_Engine.h"
#include "WG_EntityManager.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "SDL_image.h"
#include "components/WG_MeshRenderable.h"

// Supported GLFT components:
//    Nodes
//    mesh/accessor/buffer view/buffer
//    material (texture only)/texture/image

// Eg - we can really only import a tree of meshes with textures on them. No lights/camera/animations/PBR material/etc
void WG_GltfLoader::LoadGltf(const std::string& path)
{
    if (RootNode.lock()) GetEntityManager()->RemoveEntity(RootNode.lock());

    RootNode = GetEntity()->MakeChild<WG_Transform>();

    std::shared_ptr<tinygltf::Model> model = LoadGltfFile(path);
    tinygltf::Scene scene = model->scenes[model->defaultScene];

    for(int nodeIdx : scene.nodes)
    {
        LoadNode(RootNode.lock(), model, model->nodes[nodeIdx]);
    }
}


void WG_GltfLoader::LoadNode(const std::shared_ptr<WG_Entity>& parent, const std::shared_ptr<tinygltf::Model>& model, tinygltf::Node& node)
{
    // make a WG_Entity w/transform for the node
    std::shared_ptr<WG_Entity> self = parent->MakeChild<WG_Transform>(node.name);

    // set the child's rotation/scale/translation/matrix
    if (!node.matrix.empty())
    {
        // TODO: Check if the ordering of these matrices event matches, or if I have to transpose
        glm::mat4 matrix = glm::make_mat4(node.matrix.data());
        glm::vec3 _skew;
        glm::vec4 _perspective;
        glm::decompose(matrix, self->GetTransform()->Scale, self->GetTransform()->Rotation, self->GetTransform()->Position, _skew, _perspective);
    }
    else
    {
        if (!node.rotation.empty()) self->GetTransform()->Rotation = glm::make_quat(node.rotation.data());
        if (!node.scale.empty()) self->GetTransform()->Scale = glm::make_vec3(node.scale.data());
        if (!node.translation.empty()) self->GetTransform()->Position = glm::make_vec3(node.translation.data());
    }

    // if has mesh, add WG_MeshRenderable and load the mesh
    if (node.mesh != -1)
    {
        std::shared_ptr<WG_Entity> meshEntity = self->MakeChild<WG_Transform>(std::format("mesh_{}", model->meshes[node.mesh].name));
        for(int primitiveIdx = 0; primitiveIdx < model->meshes[node.mesh].primitives.size(); primitiveIdx++)
        {
            LoadPrimitive(
                meshEntity->MakeChild<WG_Transform, WG_MeshRenderable>(std::format("mesh_primitive_{}", primitiveIdx)),
                model,
                model->meshes[node.mesh].primitives[primitiveIdx]);
        }
    }

    // recurse through all children
    for(int childIdx: node.children)
    {
        LoadNode(self, model, model->nodes[childIdx]);
    }

}

void WG_GltfLoader::LoadPrimitive(const std::shared_ptr<WG_Entity>& self, const std::shared_ptr<tinygltf::Model>& model, tinygltf::Primitive& primitive)
{
    std::shared_ptr<WG_MeshRenderable> renderable = self->EnsureComponent<WG_MeshRenderable>();

    // Get Vertex Buffer
    std::vector<glm::vec3> xyzBuffer = LoadAccessor_Vec3_Float(model, model->accessors[primitive.attributes["POSITION"]]);
    std::vector<glm::vec2> uvBuffer = LoadAccessor_Vec2_Float(model, model->accessors[primitive.attributes["TEXCOORD_0"]]);

    std::vector<GLfloat> xyzuvBuffer;
    for (int i = 0; i < xyzBuffer.size(); i++)
    {
        xyzuvBuffer.push_back(xyzBuffer[i].x);
        xyzuvBuffer.push_back(xyzBuffer[i].y);
        xyzuvBuffer.push_back(xyzBuffer[i].z);
        xyzuvBuffer.push_back(uvBuffer[i].x);
        xyzuvBuffer.push_back(uvBuffer[i].y);
    }

    // Get Element Buffer
    std::vector<GLuint> elementBuffer = LoadAccessor_Scalar_UnsignedShort(model, model->accessors[primitive.indices]);
    renderable->LoadMesh(xyzuvBuffer, elementBuffer);


    // Get Texture (we only support one texture per primitive with no PBR properties)
    // TODO: support on disk textures as well as in memory textures
    int textureIdx = model->materials[primitive.material].pbrMetallicRoughness.baseColorTexture.index;
    int srcIdx = model->textures[textureIdx].source;
    tinygltf::Image tgImg = model->images[srcIdx];
    std::vector<unsigned char> imageBufferSlice = LoadBufferView(model, model->bufferViews[tgImg.bufferView]);
    SDL_RWops* sdl_mem_wrapper = SDL_RWFromMem(imageBufferSlice.data(), static_cast<int>(imageBufferSlice.size()));
    std::string extension = tgImg.mimeType.substr(tgImg.mimeType.find('/'), tgImg.mimeType.size());
    SDL_Surface* imgSurface = IMG_LoadTyped_RW(sdl_mem_wrapper, false, extension.c_str());
    renderable->LoadTexture(imgSurface);
}

template<int componentType, int componentSize>
void WG_GltfLoader::VerifyAccessor(tinygltf::Accessor& accessor)
{
    bool matches = accessor.componentType == componentType && accessor.type == componentSize;
    if (!matches) throw std::runtime_error("Accessor type or size was not as expected");
}
std::vector<glm::vec3> WG_GltfLoader::LoadAccessor_Vec3_Float(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor)
{
    VerifyAccessor<TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3>(accessor);
    std::vector<unsigned char> buffer = LoadBufferView(model, model->bufferViews[accessor.bufferView]);
    std::vector<glm::vec3> outVec;

    // 3 * 4 (float = 4, vec3 = 3)
    for (int i = 0; i < buffer.size(); i += 12)
    {
        unsigned char x[] = {buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]};
        unsigned char y[] = {buffer[i+4], buffer[i+5], buffer[i+6], buffer[i+7]};
        unsigned char z[] = {buffer[i+8], buffer[i+9], buffer[i+10], buffer[i+11]};
        glm::vec3 next;
        memcpy(&next.x, &x, 4);
        memcpy(&next.y, &y, 4);
        memcpy(&next.z, &z, 4);
        outVec.push_back(next);
    }
    return outVec;
}
std::vector<glm::vec2> WG_GltfLoader::LoadAccessor_Vec2_Float(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor)
{
    VerifyAccessor<TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC2>(accessor);
    std::vector<unsigned char> buffer = LoadBufferView(model, model->bufferViews[accessor.bufferView]);
    std::vector<glm::vec2> outVec;

    // 2 * 4 (float = 4, vec3 = 3)
    for (int i = 0; i < buffer.size(); i += 8)
    {
        unsigned char x[] = {buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]};
        unsigned char y[] = {buffer[i+4], buffer[i+5], buffer[i+6], buffer[i+7]};
        glm::vec2 next;
        memcpy(&next.x, &x, 4);
        memcpy(&next.y, &y, 4);
        outVec.push_back(next);
    }
    return outVec;
}
std::vector<GLuint> WG_GltfLoader::LoadAccessor_Scalar_UnsignedShort(const std::shared_ptr<tinygltf::Model>& model, tinygltf::Accessor& accessor)
{
    VerifyAccessor<TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, TINYGLTF_TYPE_SCALAR>(accessor);
    std::vector<unsigned char> buffer = LoadBufferView(model, model->bufferViews[accessor.bufferView]);
    std::vector<GLuint> outVec;

    // 2 * 4 (unsigned short = 2, scalar = 1)
    for (int i = 0; i < buffer.size(); i += 2)
    {
        unsigned char val[] = {buffer[i], buffer[i+1]};
        unsigned short next;
        memcpy(&next, &val, 2);
        outVec.push_back(next);
    }
    return outVec;
}


std::vector<unsigned char> WG_GltfLoader::LoadBufferView(const std::shared_ptr<tinygltf::Model>& model, tinygltf::BufferView& bufferView)
{
    std::vector<unsigned char> imageBuffer = model->buffers[bufferView.buffer].data;
    return { imageBuffer.begin() + bufferView.byteOffset, imageBuffer.begin() + bufferView.byteOffset + bufferView.byteLength };
}

std::shared_ptr<tinygltf::Model> WG_GltfLoader::LoadGltfFile(const std::string& path)
{

    std::shared_ptr<tinygltf::Model> model = std::make_shared<tinygltf::Model>();
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warn;

    bool success = loader.LoadASCIIFromFile(model.operator->(), &error, &warn, path);

    if (!warn.empty())
    {
        spdlog::warn("tinygltf load warning: {}", warn);
    }

    if (!error.empty())
    {
        std::string errorMsg = std::format("tinygltf load error: {}", error);
        spdlog::error(errorMsg);
        throw std::runtime_error(errorMsg);
    }

    if (!success)
    {
        std::string msg = "tinygltf parse failed";
        spdlog::error(msg);
        throw std::runtime_error(msg);
    }

    return model;
}

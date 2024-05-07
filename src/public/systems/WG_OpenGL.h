#pragma once

#include "GL/glew.h"
#include <GL/gl.h>
#include <variant>
#include <array>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include "WG_System.h"

class WG_Transform;

template<typename ArrayType>
concept IsGLBufferDataType = std::same_as<GLfloat, ArrayType> || std::same_as<GLint, ArrayType> || std::same_as<GLuint, ArrayType>;

template<typename ...CreationArgs>
struct WG_Opengl_Handle {
    std::string Name;
    GLuint Id;

    virtual void OnLoad(CreationArgs...) = 0;
    virtual void OnUnload() = 0;
};

template<typename HandleType, typename ...CreationArgs>
struct WG_Opengl_HandleCollection {
    std::map<std::string, std::shared_ptr<HandleType>> NameIndex;
    std::map<GLuint, std::shared_ptr<HandleType>> IdIndex;
    std::optional<std::shared_ptr<HandleType>> Get(GLuint handleId);
    std::optional<std::shared_ptr<HandleType>> Get(const std::string& handleName);
    std::shared_ptr<HandleType> Load(const std::string& name, CreationArgs...);
    std::shared_ptr<HandleType> Upsert(const std::string& name, CreationArgs...);
    void Unload(std::shared_ptr<HandleType>);
    void UnloadAll();

    struct Iterator {
        explicit Iterator(std::map<std::string, std::shared_ptr<HandleType>>::iterator inner): Inner(inner) {}
        std::shared_ptr<HandleType> operator*() const { return std::shared_ptr<HandleType>(Inner->second); }
        Iterator& operator++() { ++Inner; return *this; }
        bool operator!=(const Iterator& other) const { return Inner != other.Inner; }

    private:
        std::map<std::string, std::shared_ptr<HandleType>>::iterator Inner;
    };
    Iterator begin() {
        return Iterator{NameIndex.begin()};
    }
    Iterator end() {
        return Iterator{NameIndex.end()};
    }
};

struct WG_Opengl_Shader : public WG_Opengl_Handle<GLenum, const std::string&> {
    void OnLoad(GLenum, const std::string&) override;
    void OnUnload() override;
};

struct WG_Opengl_Buffer : public WG_Opengl_Handle<GLenum, const void*, int, int> {
    GLenum BufferType;
    void OnLoad(GLenum, const void*, int, int) override;
    void OnUnload() override;
};
struct WG_Opengl_VertexArray : public WG_Opengl_Handle<const std::function<void()>&> {
    void OnLoad(const std::function<void()>&) override;
    void OnUnload() override;
};
struct WG_Opengl_Program : public WG_Opengl_Handle<const std::vector<GLuint>&, const std::vector<std::string>&> {
    std::map<std::string, GLuint> Attributes;
    void OnLoad(const std::vector<GLuint>&, const std::vector<std::string>&) override;
    void OnUnload() override;
};
struct WG_Opengl_Texture : public WG_Opengl_Handle<SDL_Surface*> {
    void OnLoad(SDL_Surface*) override;
    void OnUnload() override;
};

class WG_OpenGL : public WG_System {
public:

    // SHADERS
    WG_Opengl_HandleCollection<WG_Opengl_Shader, GLenum, const std::string&> Shaders;
    WG_Opengl_HandleCollection<WG_Opengl_Buffer, GLenum, const void*, int, int> Buffers;
    WG_Opengl_HandleCollection<WG_Opengl_VertexArray, const std::function<void()>&> VertexArrays;
    WG_Opengl_HandleCollection<WG_Opengl_Program, const std::vector<GLuint>&, const std::vector<std::string>&> Programs;
    WG_Opengl_HandleCollection<WG_Opengl_Texture, SDL_Surface*> Textures;

    template<IsGLBufferDataType ArrayType>
    std::shared_ptr<WG_Opengl_Buffer> LoadBuffer(const std::string& bufferName, GLenum bufferType, const std::vector<ArrayType>& bufferData);
    std::shared_ptr<WG_Opengl_Texture> LoadTextureFile(const std::string& textureName, const std::string& imagePath);

    glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 CameraTargetVector = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 GetCameraMatrix() const;
    glm::mat4 Get2dMatrix() const;

    std::weak_ptr<WG_Transform> RootTransform;

    void Start() override;
    void Tick() override;
    void Cleanup() override;

    static void OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data);

    std::string Print();

};

#include "WG_OpenGL.impl.h"

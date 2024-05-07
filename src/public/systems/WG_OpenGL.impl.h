#pragma once
#include "WG_OpenGL.h"

#include "GL/glew.h"
#include <GL/gl.h>
#include <variant>
#include <array>
#include <vector>
#include <map>
#include <functional>
#include "WG_System.h"
#include "glm/vec3.hpp"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

template<typename HandleType, typename ...CreationArgs>
std::optional<std::shared_ptr<HandleType>> WG_Opengl_HandleCollection<HandleType, CreationArgs...>::Get(GLuint handleId)
{
    if (IdIndex.contains(handleId)) return IdIndex[handleId];
    return std::nullopt;
}

template<typename HandleType, typename ...CreationArgs>
std::optional<std::shared_ptr<HandleType>> WG_Opengl_HandleCollection<HandleType, CreationArgs...>::Get(const std::string& handleName)
{
    if (NameIndex.contains(handleName)) return NameIndex[handleName];
    return std::nullopt;
}

template<typename HandleType, typename ...CreationArgs>
std::shared_ptr<HandleType> WG_Opengl_HandleCollection<HandleType, CreationArgs...>::Load(const std::string& name, CreationArgs... args)
{
    auto elem = std::make_shared<HandleType>();
    elem->Name = name;
    elem->OnLoad(std::forward<decltype(args)>(args)...);
    NameIndex[elem->Name] = elem;
    IdIndex[elem->Id] = elem;
    return elem;
}

template<typename HandleType, typename ...CreationArgs>
std::shared_ptr<HandleType> WG_Opengl_HandleCollection<HandleType, CreationArgs...>::Upsert(const std::string& name, CreationArgs... args)
{
    auto elem = Get(name);
    if (elem.has_value()) return elem.value();
    return Load(name, std::forward<decltype(args)>(args)...);
}

template<typename HandleType, typename ...CreationArgs>
void WG_Opengl_HandleCollection<HandleType, CreationArgs...>::Unload(std::shared_ptr<HandleType> elem)
{
    elem->OnUnload();
    NameIndex.erase(elem->Name);
    IdIndex.erase(elem->Id);
}

template<typename HandleType, typename ...CreationArgs>
void WG_Opengl_HandleCollection<HandleType, CreationArgs...>::UnloadAll()
{
    std::vector<std::shared_ptr<HandleType>> toUnload;
    for(auto e : NameIndex)
    {
        toUnload.push_back(e.second);
    }
    for(auto e : toUnload)
    {
        Unload(e);
    }
}

template<IsGLBufferDataType ArrayType>
std::shared_ptr<WG_Opengl_Buffer> WG_OpenGL::LoadBuffer(const std::string& bufferName, GLenum bufferType, const std::vector<ArrayType>& bufferData)
{
    const void* bufferHead = bufferData.data();
    return Buffers.Load(bufferName, bufferType, bufferHead, sizeof(bufferData[0]), bufferData.size());
}



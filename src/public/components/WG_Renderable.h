#pragma once

#include "SDL.h"
#include "systems/WG_OpenGL.h"
#include "componentdecorators/WG_Tickable.h"
#include <memory>
#include <vector>
#include "tiny_gltf.h"

class WG_Component;
class WG_Transform;

enum class DrawMode {
    DrawMode_2d, DrawMode_3d
};

class WG_Renderable: public virtual WG_Component {

public:
    std::optional<std::weak_ptr<WG_Opengl_Shader>> FragmentShader;
    std::optional<std::weak_ptr<WG_Opengl_Shader>> VertexShader;
    std::optional<std::weak_ptr<WG_Opengl_Program>> Program;
    std::optional<std::weak_ptr<WG_Opengl_Buffer>> VertexBuffer;
    std::optional<std::weak_ptr<WG_Opengl_Buffer>> ElementBuffer;
    int ElementBufferLength = 0;
    std::optional<std::weak_ptr<WG_Opengl_VertexArray>> VertexArray;
    std::optional<std::weak_ptr<WG_Opengl_Texture>> Texture;

    std::string ShaderPrefix;
    std::weak_ptr<WG_OpenGL> OpenGL;
    std::weak_ptr<WG_Transform> Transform;

    void LoadTexture(SDL_Surface* image);
    void LoadTextureFile(const std::string& imagePath);

    void SetProps(WG_PropsMap props) override
    {
        props.WithString("texture", [this](auto s) {
            LoadTextureFile(s);
        });
    }

    void Start() override;

    virtual void Draw() {};
    virtual void PostChildDraw() {};
};


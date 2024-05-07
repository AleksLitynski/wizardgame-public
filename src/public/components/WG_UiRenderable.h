#pragma once

#include "SDL.h"
#include "systems/WG_OpenGL.h"
#include "componentdecorators/WG_Tickable.h"
#include <memory>
#include <vector>
#include "tiny_gltf.h"
#include "components/WG_Renderable.h"
#include "WG_PropsMap.h"



class WG_UiRenderable: public virtual WG_Component, public virtual WG_Renderable {

public:
    // Draw flags
    float BorderThickness = 0.0f;
    WG_Color FillColor = WG_Colors::blue;
    WG_Color BorderColor = WG_Colors::black;
    enum class UiFillType {
        COLOR,
        TEXTURE,
        NONE
    };
    UiFillType FillType = UiFillType::COLOR;
    bool RelativePosition = true;
    bool ClipContent = true;

    struct {
        std::optional<std::weak_ptr<WG_Opengl_Buffer>> VertexBuffer;
        std::optional<std::weak_ptr<WG_Opengl_Buffer>> ElementBuffer;
        int ElementBufferLength = 0;
        std::optional<std::weak_ptr<WG_Opengl_VertexArray>> VertexArray;

        std::optional<std::weak_ptr<WG_Opengl_Shader>> FragmentShader;
        std::optional<std::weak_ptr<WG_Opengl_Shader>> VertexShader;
        std::optional<std::weak_ptr<WG_Opengl_Program>> Program;
    } Outline;


    int ClipDepth();

    void Start() override;
    void Draw() override;
    void PostChildDraw() override;
    void SetProps(WG_PropsMap props) override
    {
        WG_Renderable::SetProps(props);
        props.WithBool("clip-content", [this](auto s) {
            ClipContent = s;
        });
        props.WithString("position", [this](auto s) {
            if (s == "relative")
            {
                RelativePosition = true;
            }
            if (s == "absolute")
            {
                RelativePosition = false;
            }
        });
        props.WithVec4("fill-color", [this](auto s) {
            FillColor = s;
            FillType = UiFillType::COLOR;
        });
        props.WithColor("border-color", [this](auto s) {
            BorderColor = s;
            FillType = UiFillType::COLOR;
        });
        props.WithFloat("border-thickness", [this](auto s) {
            BorderThickness = s;
        });

    }
    void InnerDraw();
    void DrawBorder();
    void LoadShape(const std::vector<glm::vec2>& ShapePoints);
    glm::mat4 GetTransform();

};


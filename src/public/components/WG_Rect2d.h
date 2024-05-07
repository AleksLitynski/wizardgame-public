#pragma once

#include <memory>
#include <vector>
#include "WG_Component.h"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "WG_Colors.h"

class WG_Rect2d: public WG_Component {

public:
    void Start() override;

    void SetProps(WG_PropsMap props) override
    {
        props.WithVec2("rect-dimensions", [this](auto s) {
            SetWidth(s.x);
            SetHeight(s.y);
        });
        props.WithVec2("rect-offset", [this](auto s) {
            SetX(s.x);
            SetY(s.y);
        });
        props.WithColor("rect-color", [this](auto s) {
            SetColor(s);
        });
    }

    void SetWidth(float width);
    void SetHeight(float height);
    void SetX(float x);
    void SetY(float y);
    void SetColor(WG_Color color);

private:
    glm::vec2 Position{0.0f, 0.0f};
    glm::vec2 Dimensions{100.0f, 100.0f};
    void UpdateMesh();
};

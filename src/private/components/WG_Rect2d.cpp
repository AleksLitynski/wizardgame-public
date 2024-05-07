#include "components/WG_Rect2d.h"

#include "WG_Entity.h"
#include "glm/gtx/string_cast.hpp"
#include "spdlog/spdlog.h"
#include "components/WG_UiRenderable.h"
#include "WG_Entity.h"
#include "WG_Colors.h"


void WG_Rect2d::Start()
{;
    auto renderable = EnsureComponent<WG_UiRenderable>();
    UpdateMesh();
}

void WG_Rect2d::SetWidth(float width)
{
    Dimensions.x = width;
    UpdateMesh();
}

void WG_Rect2d::SetHeight(float height)
{
    Dimensions.y = height;
    UpdateMesh();
}

void WG_Rect2d::SetX(float x)
{
    Position.x = x;
    UpdateMesh();
}

void WG_Rect2d::SetY(float y)
{
    Position.y = y;
    UpdateMesh();
}

void WG_Rect2d::SetColor(WG_Color color)
{
    GetEntity()->EnsureComponent<WG_UiRenderable>()->FillColor = color;
    GetEntity()->EnsureComponent<WG_UiRenderable>()->FillType = WG_UiRenderable::UiFillType::COLOR;
}

void WG_Rect2d::UpdateMesh()
{

    GetEntity()->EnsureComponent<WG_UiRenderable>()->LoadShape({
                                                                       {Position.x,                Position.y},
                                                                       {Position.x + Dimensions.x, Position.y},
                                                                       {Position.x + Dimensions.x, Position.y +
                                                                                                   Dimensions.y},
                                                                       {Position.x,                Position.y +
                                                                                                   Dimensions.y},
                                                               });

//    GetEntity()->EnsureComponent<WG_UiRenderable>()->LoadMesh({
//        // x, y
//        Position.x, Position.y,
//        Position.x + Dimensions.x, Position.y,
//        Position.x + Dimensions.x, Position.y + Dimensions.y,
//        Position.x, Position.y + Dimensions.y,
//    }, {
//        0, 1, 2,
//        0, 2, 3
//    });
}
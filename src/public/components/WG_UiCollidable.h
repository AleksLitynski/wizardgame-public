#pragma once

#include <memory>
#include <vector>
#include "components/WG_Renderable.h"
#include "WG_PropsMap.h"
#include "systems/WG_Collision2d.h"


class WG_UiCollidable: public virtual WG_Component {

public:
    std::weak_ptr<WG_Collision2d> Collision2d;
    std::weak_ptr<Shape> InnerShape;

    WG_EventEmitter<std::shared_ptr<Shape>> GetOnCollisionEnterEmitter();
    WG_EventEmitter<std::shared_ptr<Shape>> GetOnCollisionExitEmitter();
    bool Overlap(std::shared_ptr<Shape> other);
};

class WG_UiCollidableRect: public virtual WG_UiCollidable {

public:
    void Start() override;
    RectBounds GetBounds();
    WG_UiCollidableRect* SetBounds(float x, float y, float w, float h);
    WG_UiCollidableRect* SetX(float x);
    WG_UiCollidableRect* SetY(float y);
    WG_UiCollidableRect* SetW(float w);
    WG_UiCollidableRect* SetH(float h);
};

class WG_UiCollidableCircle: public virtual WG_UiCollidable {

public:
    void Start() override;
    CircleBounds GetBounds();
    WG_UiCollidableRect* SetBounds(float x, float y, float r);
    WG_UiCollidableRect* SetX(float x);
    WG_UiCollidableRect* SetY(float y);
    WG_UiCollidableRect* SetR(float r);
};


#include <vector>
#include "components/WG_UiCollidable.h"


WG_EventEmitter<std::shared_ptr<Shape>> WG_UiCollidable::GetOnCollisionEnterEmitter()
{
    return InnerShape.lock()->OnCollisionEnter;
}
WG_EventEmitter<std::shared_ptr<Shape>> WG_UiCollidable::GetOnCollisionExitEmitter()
{
    return InnerShape.lock()->OnCollisionExit;
}
bool WG_UiCollidable::Overlap(std::shared_ptr<Shape> other)
{
    return InnerShape.lock()->Overlap(other);
}

void WG_UiCollidableRect::Start()
{
    Collision2d = EnsureSystem<WG_Collision2d>();
    InnerShape = Collision2d.lock()->MakeRect(0, 0, 0, 0);
}
RectBounds WG_UiCollidableRect::GetBounds()
{
    return InnerShape.lock()->BoundsData.Rect;
}

WG_UiCollidableRect* WG_UiCollidableRect::SetBounds(float x, float y, float w, float h) {
    Collision2d.lock()->UpdateRect(InnerShape.lock(), x, y, w, h);
    return this;
}
WG_UiCollidableRect* WG_UiCollidableRect::SetX(float x) {
    auto b = GetBounds();
    Collision2d.lock()->UpdateRect(InnerShape.lock(), x, b.Y, b.W, b.H);
    return this;
}
WG_UiCollidableRect* WG_UiCollidableRect::SetY(float y) {
    auto b = GetBounds();
    Collision2d.lock()->UpdateRect(InnerShape.lock(), b.X, y, b.W, b.H);
    return this;
}
WG_UiCollidableRect* WG_UiCollidableRect::SetW(float w) {
    auto b = GetBounds();
    Collision2d.lock()->UpdateRect(InnerShape.lock(), b.X, b.Y, w, b.H);
    return this;
}
WG_UiCollidableRect* WG_UiCollidableRect::SetH(float h) {
    auto b = GetBounds();
    Collision2d.lock()->UpdateRect(InnerShape.lock(), b.X, b.Y, b.W, h);
    return this;
}


void WG_UiCollidableCircle::Start() {
    Collision2d = EnsureSystem<WG_Collision2d>();
    InnerShape = Collision2d.lock()->MakeCircle(0, 0, 0);
}
CircleBounds WG_UiCollidableCircle::GetBounds() {
    return InnerShape.lock()->BoundsData.Circle;
}
WG_UiCollidableRect* WG_UiCollidableCircle::SetBounds(float x, float y, float r)
{

}
WG_UiCollidableRect WG_UiCollidableCircle::SetX(float x) {}
WG_UiCollidableRect WG_UiCollidableCircle::SetY(float y) {}
WG_UiCollidableRect WG_UiCollidableCircle::SetR(float r) {}
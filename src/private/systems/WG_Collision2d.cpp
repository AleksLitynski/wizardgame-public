#include "systems/WG_Collision2d.h"
#include "spdlog/spdlog.h"

#include <set>
#include <glm/geometric.hpp>
#include <glm/gtx/closest_point.hpp>


bool Shape::Overlap(std::shared_ptr<Shape> other)
{
    Shape* otherPtr = other.get();
    // rect / rect
    if (this->BoundsType == Rect && otherPtr->BoundsType == Rect)
    {
        auto leftRect = this->BoundsData.Rect.X <= other->BoundsData.Rect.X ? this->BoundsData.Rect : otherPtr->BoundsData.Rect;
        auto rightRect = this->BoundsData.Rect.X > other->BoundsData.Rect.X ? this->BoundsData.Rect : otherPtr->BoundsData.Rect;
        if (leftRect.Right() < rightRect.X) return false;

        auto bottomRect = this->BoundsData.Rect.Y <= other->BoundsData.Rect.Y ? this->BoundsData.Rect : otherPtr->BoundsData.Rect;
        auto topRect = this->BoundsData.Rect.X > other->BoundsData.Rect.X ? this->BoundsData.Rect : otherPtr->BoundsData.Rect;
        if (bottomRect.Top() < topRect.X) return false;

        return true;
    }
    // circle / circle
    if (this->BoundsType == Circle && other->BoundsType == Circle)
    {
        auto c1 = this->BoundsData.Circle;
        auto c2 = other->BoundsData.Circle;
        auto combinedRadius = c1.R + c2.R;
        auto thisPos = glm::vec2(c1.X, c1.Y);
        auto otherPos = glm::vec2(c2.X, c2.Y);
        return glm::distance(thisPos, otherPos) <= combinedRadius;
    }
    // rect / circle
    if (this->BoundsType == Circle && other->BoundsType == Rect || this->BoundsType == Rect && other->BoundsType == Circle)
    {
        // https://stackoverflow.com/questions/401847/circle-rectangle-collision-detection-intersection
        auto rect = this->BoundsType == Rect ? this->BoundsData.Rect : otherPtr->BoundsData.Rect;
        auto circle = this->BoundsType == Circle ? this->BoundsData.Circle : otherPtr->BoundsData.Circle;

        // check if the midpoint of the cirlce is in the rect
        auto circleInXBounds = rect.X < circle.X && rect.Right() > circle.X;
        auto circleInYBounds = rect.Y < circle.Y && rect.Top() > circle.Y;
        if (circleInXBounds && circleInYBounds) return true;

        // otherwise, check if any edge of the rect crosses the circle
        auto pt = glm::vec2(circle.X, circle.Y);
        auto bl = glm::vec2(rect.X, rect.Y);
        auto br = glm::vec2(rect.Right(), rect.Y);
        auto tr = glm::vec2(rect.Right(), rect.Top());
        auto tl = glm::vec2(rect.X, rect.Top());
        return glm::abs(glm::distance(pt, glm::closestPointOnLine(pt, bl, br))) < circle.R
            || glm::abs(glm::distance(pt, glm::closestPointOnLine(pt, br, tr))) < circle.R
            || glm::abs(glm::distance(pt, glm::closestPointOnLine(pt, tr, tl))) < circle.R
            || glm::abs(glm::distance(pt, glm::closestPointOnLine(pt, tl, bl))) < circle.R;
    }
    return false;
}

std::shared_ptr<Shape> WG_Collision2d::MakeCircle(float x, float y, float r)
{
    auto shape = std::make_shared<Shape>();
    shape->BoundsType = Shape::BoundsType::Circle;
    shape->BoundsData.Circle.X = x;
    shape->BoundsData.Circle.Y = y;
    shape->BoundsData.Circle.R = r;
    InsertShape(shape, true);
    return shape;
}

std::shared_ptr<Shape> WG_Collision2d::MakeRect(float x, float y, float w, float h)
{
    auto shape = std::make_shared<Shape>();
    shape->BoundsType = Shape::BoundsType::Rect;
    shape->BoundsData.Rect.X = x;
    shape->BoundsData.Rect.Y = y;
    shape->BoundsData.Rect.W = w;
    shape->BoundsData.Rect.H = h;
    InsertShape(shape, true);
    return shape;
}

void WG_Collision2d::InsertShape(std::shared_ptr<Shape> shape, bool raiseEvents)
{
    RootNode.TryInsertShape(shape, true);
    if (raiseEvents)
    {
        for(auto other : GetCollisions(shape))
        {
            shape->OnCollisionEnter.Emit(other);
            other->OnCollisionEnter.Emit(shape);
        }
    }
}

bool QuadtreeNode::TryInsertShape(std::shared_ptr<Shape> shape, bool mustInsert)
{
    if (FitsShape(shape))
    {
        Expand();
        for (auto cn : ChildNodes()) {
            if (cn->TryInsertShape(shape, false)) return true;
        }
        TryCollapse();
        Shapes.push_back(shape);
        return true;
    }
    if (mustInsert)
    {
        Shapes.push_back(shape);
        return true;
    }
    return false;
}

void QuadtreeNode::Expand()
{
    if (Expanded) return;
    auto hW = Bounds.W / 2;
    auto hH = Bounds.H / 2;
    auto setBounds = [](QuadtreeNode* node, float x, float y, float w, float h) {
        node->Bounds.X = x; node->Bounds.Y = y;
        node->Bounds.W = w; node->Bounds.H = h;
    };
    TopLeft = new QuadtreeNode();
    TopRight = new QuadtreeNode();
    BottomLeft = new QuadtreeNode();
    BottomRight = new QuadtreeNode();
    setBounds(TopLeft, Bounds.X, Bounds.Y + hH, hW, hH);
    setBounds(TopRight, Bounds.X + hW, Bounds.Y + hH, hW, hH);
    setBounds(BottomLeft, Bounds.X, Bounds.Y, hW, hH);
    setBounds(BottomRight, Bounds.X + hW, Bounds.Y, hW, hH);
    Expanded = true;
}

void QuadtreeNode::TryCollapse()
{
    if (!Expanded) return;
    bool allEmpty = true;
    for (auto n : ChildNodes()) {
        n->TryCollapse();
        if (n->Expanded) allEmpty = false;
    }
    if (allEmpty)
    {
        for (auto n : ChildNodes()) delete n;
        Expanded = false;
    }
}

QuadtreeNode* QuadtreeNode::FindCurrentNode(std::shared_ptr<Shape> shape)
{
    if (FitsShape(shape))
    {
        if (!Expanded) return this;
        for (auto cn : ChildNodes())
        {
            auto node = cn->FindCurrentNode(shape);
            if (node != nullptr) return node;
        }
    }
    return nullptr;
}

void WG_Collision2d::RemoveShape(std::shared_ptr<Shape> shape, bool raiseEvents)
{
    std::set<std::shared_ptr<Shape>> collisions;
    if (raiseEvents)
    {
        collisions = GetCollisions(shape);
    }

    auto currentNode = RootNode.FindCurrentNode(shape);
    if (currentNode != nullptr)
    {
        std::erase_if(currentNode->Shapes, [shape](std::shared_ptr<Shape> n) {
            return shape == n;
        });
    }

    RootNode.TryCollapse();
    if (raiseEvents)
    {
        for (auto c : collisions) c->OnCollisionExit.Emit(shape);
    }
}

void WG_Collision2d::DestroyShape(std::shared_ptr<Shape> shape)
{
    RemoveShape(shape, true);
}

std::shared_ptr<Shape> WG_Collision2d::UpdateShape(std::shared_ptr<Shape> shape, std::function<void(std::shared_ptr<Shape>)> update)
{
    auto PreCollisions = GetCollisions(shape);
    RemoveShape(shape, false);

    update(shape);

    InsertShape(shape, false);
    auto PostCollisions = GetCollisions(shape);

    for (auto pc : PreCollisions)
    {
        if (!PostCollisions.contains(pc)) pc->OnCollisionExit.Emit(shape);
    }
    for (auto pc : PostCollisions)
    {
        if (!PreCollisions.contains(pc)) pc->OnCollisionEnter.Emit(shape);
    }

    return shape;
}

std::shared_ptr<Shape> WG_Collision2d::UpdateCircle(std::shared_ptr<Shape> shape, float x, float y, float r)
{
    return UpdateShape(shape, [x, y, r](auto s)
    {
        s->BoundsData.Circle.X = x;
        s->BoundsData.Circle.Y = y;
        s->BoundsData.Circle.R = r;
    });
}

std::shared_ptr<Shape> WG_Collision2d::UpdateRect(std::shared_ptr<Shape> shape, float x, float y, float w, float h)
{
    return UpdateShape(shape, [x, y, w, h](auto s)
    {
        s->BoundsData.Rect.X = x;
        s->BoundsData.Rect.Y = y;
        s->BoundsData.Rect.W = w;
        s->BoundsData.Rect.H = h;
    });
}

std::set<std::shared_ptr<Shape>> WG_Collision2d::GetCollisions(std::shared_ptr<Shape> shape)
{
    // start at the root and work our way towards wherever this shape goes
    std::set<std::shared_ptr<Shape>> collisions;
    RootNode.GetCollisions(collisions, shape);
    return collisions;
}

std::set<std::shared_ptr<Shape>> WG_Collision2d::ProbePoint(float X, float Y)
{
    return ProbeCircle(X, Y, 0.1f);
}

std::set<std::shared_ptr<Shape>> WG_Collision2d::ProbeCircle(float X, float Y, float R)
{
    auto shape = std::make_shared<Shape>();
    shape->BoundsType = Shape::Circle;
    shape->BoundsData.Circle.X = X;
    shape->BoundsData.Circle.Y = Y;
    shape->BoundsData.Circle.R = R;
    return GetCollisions(shape);
}

std::set<std::shared_ptr<Shape>> WG_Collision2d::ProbeRect(float X, float Y, float W, float H)
{
    auto shape = std::make_shared<Shape>();
    shape->BoundsType = Shape::Rect;
    shape->BoundsData.Rect.X = X;
    shape->BoundsData.Rect.Y = Y;
    shape->BoundsData.Rect.W = W;
    shape->BoundsData.Rect.H = H;
    return GetCollisions(shape);
}

void QuadtreeNode::GetCollisions(std::set<std::shared_ptr<Shape>>& collisions, std::shared_ptr<Shape> shape)
{
    if (FitsShape(shape))
    {
        for (auto other : Shapes)
        {
            if (shape != other && shape->Overlap(other))
            {
                collisions.insert(shape);
            }
        }
        if (Expanded)
        {
            TopLeft->GetCollisions(collisions, shape);
            TopRight->GetCollisions(collisions, shape);
            BottomLeft->GetCollisions(collisions, shape);
            BottomRight->GetCollisions(collisions, shape);
        }
    }
}

WG_Collision2d::WG_Collision2d()
{
    RootNode.Bounds.X = -800;
    RootNode.Bounds.Y = -600;
    RootNode.Bounds.W = 1600;
    RootNode.Bounds.H = 1200;
}

bool QuadtreeNode::FitsShape(std::shared_ptr<Shape> shape)
{
    if (shape->BoundsType == Shape::Circle)
    {
        auto c = shape->BoundsData.Circle;
        return c.X - c.R > Bounds.X && c.X + c.R < Bounds.Right() && c.Y - c.R > Bounds.Y && c.Y + c.R < Bounds.Top();
    }
    if (shape->BoundsType == Shape::Rect)
    {
        auto r = shape->BoundsData.Rect;
        return r.X > Bounds.X && r.Y > Bounds.Y && r.Top() < Bounds.Top() && r.Right() < Bounds.Right();
    }
    return false;
}

std::set<std::shared_ptr<Shape>> WG_Collision2d::GetAllShapes()
{
    std::set<std::shared_ptr<Shape>> shapes;
    RootNode.GetAllShapes(shapes);
    return shapes;
}

void QuadtreeNode::GetAllShapes(std::set<std::shared_ptr<Shape>>& shapes)
{
    for (auto s : Shapes) shapes.insert(s);
    if (!Expanded) return;
    for (auto cn : ChildNodes()) cn->GetAllShapes(shapes);
}
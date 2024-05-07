#pragma once

#include <array>

#include "SDL.h"
#include <chrono>
#include <set>
#include "WG_EventEmitter.h"
#include "WG_System.h"
#include "glm/vec2.hpp"

class WG_Entity;
class WG_Collision2d;
struct QuadtreeNode;

struct RectBounds {
    float X, Y, W, H;
    float Top() const { return Y + H; }
    float Right() const { return X + W; }
};
struct CircleBounds {
    float X, Y, R;
};

struct Shape: std::enable_shared_from_this<Shape>
{
    union BoundsData
    {
        RectBounds Rect;
        CircleBounds Circle;
    } BoundsData;
    enum BoundsType
    {
        Rect,
        Circle,
    } BoundsType;

    WG_EventEmitter<std::shared_ptr<Shape>> OnCollisionEnter;
    WG_EventEmitter<std::shared_ptr<Shape>> OnCollisionExit;

    bool Overlap(std::shared_ptr<Shape> other);
};

struct QuadtreeNode
{
    std::weak_ptr<WG_Collision2d> CollisionSystem;
    bool Expanded = false;
    QuadtreeNode* TopLeft;
    QuadtreeNode* TopRight;
    QuadtreeNode* BottomLeft;
    QuadtreeNode* BottomRight;
    std::array<QuadtreeNode*, 4> ChildNodes() {
        return {TopLeft, TopRight, BottomLeft, BottomRight};
    }
    std::vector<std::shared_ptr<Shape>> Shapes;
    RectBounds Bounds;
    bool FitsShape(std::shared_ptr<Shape> shape);
    void GetCollisions(std::set<std::shared_ptr<Shape>>& collisions, std::shared_ptr<Shape> shape);
    void GetAllShapes(std::set<std::shared_ptr<Shape>>& collisions);
    bool TryInsertShape(std::shared_ptr<Shape> shape, bool mustInsert);
    void Expand();
    void TryCollapse();
    QuadtreeNode* FindCurrentNode(std::shared_ptr<Shape> shape);
};

class WG_Collision2d: public WG_System {
public:
    WG_Collision2d();
    std::shared_ptr<Shape> MakeCircle(float x, float y, float r);
    std::shared_ptr<Shape> MakeRect(float x, float y, float w, float h);
    QuadtreeNode RootNode;

    void DestroyShape(std::shared_ptr<Shape> shape);
    std::shared_ptr<Shape> UpdateCircle(std::shared_ptr<Shape> shape, float x, float y, float r);
    std::shared_ptr<Shape> UpdateRect(std::shared_ptr<Shape> shape, float x, float y, float w, float h);
    std::shared_ptr<Shape> UpdateShape(std::shared_ptr<Shape> shape, std::function<void(std::shared_ptr<Shape>)>);
    std::set<std::shared_ptr<Shape>> GetCollisions(std::shared_ptr<Shape> shape);
    std::set<std::shared_ptr<Shape>> ProbePoint(float X, float Y);
    std::set<std::shared_ptr<Shape>> ProbeCircle(float X, float Y, float R);
    std::set<std::shared_ptr<Shape>> ProbeRect(float X, float Y, float W, float H);
    std::set<std::shared_ptr<Shape>> GetAllShapes();
private:
    void InsertShape(std::shared_ptr<Shape> shape, bool raiseEvents);
    void RemoveShape(std::shared_ptr<Shape> shape, bool raiseEvents);
};


// What do I want to know?
//   be able to ask what is currently overlapping a given Bounds
//   be able to receive an event when things overlap our shape

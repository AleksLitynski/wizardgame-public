
#include "scenes/WG_SampleScenes.h"

#include <memory>
#include "spdlog/spdlog.h"
#include "WG_Engine.h"
#include "systems/WG_Collision2d.h"


void sampleCollisions(std::shared_ptr<WG_Engine> engine)
{
    auto cm = engine->Systems.Ensure<WG_Collision2d>();

    auto c1 = cm->MakeCircle(0, 0, 5);
    c1->OnCollisionEnter.Subscribe([](auto other) {
        spdlog::info("c1 hit: {}, {}, {}", other->BoundsData.Circle.X, other->BoundsData.Circle.Y, other->BoundsData.Circle.R);
        });
    auto c2 = cm->MakeCircle(0, 1, 5);

    spdlog::info("c1 collision: {}", cm->GetCollisions(c1).size());
}

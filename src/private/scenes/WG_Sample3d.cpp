
#include "scenes/WG_SampleScenes.h"

#include <memory>

#include "WG_EntityManager.h"
#include "WG_Macros.h"
#include "components/WG_Transform.h"
#include "components/WG_GltfLoader.h"
#include "componentdecorators/WG_Tickable.h"
#include "spdlog/spdlog.h"


void sample3d(std::shared_ptr<WG_Engine> engine)
{
    std::shared_ptr<WG_EntityManager> em = engine->GetEntityManager();

    // Create the first entity of our game
    auto meshesRoot = em->MakeChild<WG_Transform>()->SetProps({
        {"position", "0, 0, 15"},
        });
    ANONCOMPD(meshesRoot, public WG_Tickable{
        void Tick(float delta) override {
            auto Transform = EnsureComponent<WG_Transform>();
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0015f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        });

    std::shared_ptr<WG_Entity> monkey = meshesRoot->MakeChild<WG_GltfLoader>()->SetProps({
        {"gltf-mesh", "assets/bad_monkey.gltf"},
        {"position", "3, 0, 0"},
        });
    ANONCOMPD(monkey, public WG_Tickable{
        void Tick(float delta) override {
            auto Transform = EnsureComponent<WG_Transform>();
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 0.0f, 1.0f));
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 1.0f, 0.0f));
            Transform->Scale = glm::vec3(sinf(0.005f * static_cast<float>(SDL_GetTicks64())) * 0.5f + 0.5f, sinf(0.005f * static_cast<float>(SDL_GetTicks64())) * 0.5f + 0.5f, 1.0f);
        }
        });

    std::shared_ptr<WG_Entity> cube = meshesRoot->MakeChild<WG_GltfLoader>()->SetProps({
        {"gltf-mesh", "assets/test_cube.gltf"},
        {"position", "-3, 0, 0"},
        });
    ANONCOMPD(cube, public WG_Tickable{
        void Tick(float delta) override {
            auto Transform = EnsureComponent<WG_Transform>();
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 0.0f, 1.0f));
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        });
}
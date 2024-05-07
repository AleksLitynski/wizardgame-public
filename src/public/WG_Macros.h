#pragma once
#include "map.h"

// Use shadowing to redefine a class std::weak_ptr variable to it's std::shared_ptr form to be used within some scope
#define UNLOCK(BINDNAME, NAME) auto (BINDNAME) = this->NAME.lock();
// #define UNLOCKS(...) MAP(UNLOCK, __VA_ARGS__)

// If you get an error about name redefinition, just give the anonymous class a name and the compiler will sort out what's happening. Name can be reused, so I recommend '_' as a name
// Anonymous component, anonymous component with decorators (D)
#define ANONCOMPD(ENTITY, ...) [&ENTITY]{ typedef struct _: public virtual WG_Component, __VA_ARGS__ AnonComponentClass; return ENTITY->EnsureComponent<AnonComponentClass>(); }()
#define ANONCOMP(ENTITY, ...) [&ENTITY]{ typedef struct _: public virtual WG_Component __VA_ARGS__ AnonComponentClass; return ENTITY->EnsureComponent<AnonComponentClass>(); }()

/* example:
      auto AnonComp = ANONCOMP(entity, struct _: public virtual WG_Component, public WG_Tickable {
        void Start() override {
            // TESTING CONFIGURATION
            auto Transform = EnsureComponent<WG_Transform>();
            Transform->Position.z += 5.0f;
            spdlog::info("The anoncomp started");
            auto Renderable = EnsureComponent<WG_MeshRenderable>();
            Renderable->LoadTextureFile(std::filesystem::current_path().string() + "/assets/get_smoked_420.jpg");
            Renderable->LoadMesh(std::vector<GLfloat>{
                //  X, Y, Z               U, V
                 1.0f,  1.0f, 0.0f,    1.0f, 1.0f, // top right
                 1.0f,  -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
                -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, // bottom left
                -1.0f,  1.0f, 0.0f,    0.0f, 1.0f, // top left
            }, std::vector<GLuint>{
                0, 1, 3,  // first Triangle
                1, 2, 3   // second Triangle
            });

        }
        void Tick(float delta) override {
            auto Transform = EnsureComponent<WG_Transform>();
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 0.0f, 1.0f));
            Transform->Rotation = glm::rotate(Transform->Rotation, delta * 0.0005f, glm::vec3(0.0f, 1.0f, 0.0f));
//            Transform->Position.y = sinf(0.005f * static_cast<float>(SDL_GetTicks64())) * 0.5f;
            Transform->Scale = glm::vec3(sinf(0.005f * static_cast<float>(SDL_GetTicks64())) * 0.5f + 0.5f, sinf(0.005f * static_cast<float>(SDL_GetTicks64())) * 0.5f + 0.5f, 1.0f);
        }
    }); */
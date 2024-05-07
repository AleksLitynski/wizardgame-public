// #define SDL_MAIN_HANDLED
#include "main.h"

#include "spdlog/spdlog.h"
#include "WG_Engine.h"
#include "WG_EntityManager.h"
#include "scenes/WG_SampleScenes.h"
#include "systems/WG_OpenGL.h"

int main(int argc, char *argv[]) {
    try
    {
        unsafeMain();
    }
    catch (std::exception& e)
    {
        spdlog::error("TOP LEVEL ERROR: {}", e.what());
        return 1;
    }
    return 0;
}


void unsafeMain()
{
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Wizard Game Started");

    std::shared_ptr<WG_Engine> engine = std::make_shared<WG_Engine>();
    engine->Start();

    sample2d(engine);

    engine->GetEntityManager()->Debug_LogEntityTree();
    spdlog::info(engine->Systems.Get<WG_OpenGL>().value()->Print());
    engine->Run();
    engine->Cleanup();
}

// Store weak pointers, but return shared_ptr from functions. It's fine in scope

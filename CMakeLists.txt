cmake_minimum_required(VERSION 3.26)

project(WizardGame CXX)
set(CMAKE_CXX_STANDARD 20)

# Logs a LOT of debug information about targets:
# set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

set(CMAKE_BUILD_TYPE Debug) # Debug or RelWithDebInfo (releae with debug info, eg optimized, but with symbols)

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_PREFIX_PATH vendor/SDL2-2.28.2)
set(BUILD_SHARED_LIBS OFF)

# https://github.com/libsdl-org/SDL/issues/7267
set(SDL2_DISABLE_INSTALL OFF)
set(SDL2IMAGE_INSTALL OFF)
set(SDL2TTF_INSTALL OFF)

find_package(OpenGL REQUIRED)

# https://wiki.libsdl.org/SDL2/README/cmake
add_subdirectory(vendor/SDL2-2.28.2 EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL2_image-2.6.3 EXCLUDE_FROM_ALL)

# Tell SDL2_ttf the path to freetype
set(SDL2TTF_VENDORED "${PROJECT_SOURCE_DIR}/vendor/freetype-VER-2-13-1")
add_subdirectory(vendor/SDL2_ttf-2.20.2 EXCLUDE_FROM_ALL)

set(GLEW_USE_STATIC_LIBS TRUE)
add_subdirectory(vendor/glew-2.1.0/build/cmake EXCLUDE_FROM_ALL)

set(SPDLOG_BUILD_SHARED OFF)
add_subdirectory(vendor/spdlog-1.12.0 EXCLUDE_FROM_ALL)

add_subdirectory(vendor/glm-0.9.9.8 EXCLUDE_FROM_ALL)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(vendor/tinygltf-2.8.17)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Lets us set these options via variables
set(EARCUT_BUILD_BENCH OFF)
set(EARCUT_BUILD_TESTS OFF)
set(EARCUT_BUILD_VIZ OFF)
add_subdirectory(vendor/earcut.hpp-2.2.4)


add_executable(WizardGame
        src/private/main.cpp
        src/public/main.h
        src/private/systems/WG_Text.cpp
        src/public/systems/WG_Text.h
        src/private/WG_System.cpp
        src/public/WG_System.h
        src/private/systems/WG_Image.cpp
        src/public/systems/WG_Image.h
        src/private/WG_Engine.cpp
        src/public/WG_Engine.h
        src/private/systems/WG_OpenGL.cpp
        src/public/systems/WG_OpenGL.h
        src/public/systems/WG_OpenGL.impl.h
        src/private/WG_EntityManager.cpp
        src/public/WG_EntityManager.h
        src/public/WG_EntityManager.impl.h
        src/private/WG_Entity.cpp
        src/public/WG_Entity.h
        src/public/WG_Entity.impl.h
        src/private/WG_InstancesSet.cpp
        src/public/WG_InstancesSet.h
        src/public/WG_InstancesSet.impl.h
        src/private/WG_Component.cpp
        src/public/WG_Component.h
        src/public/WG_Component.impl.h
        src/private/components/WG_Transform.cpp
        src/public/components/WG_Transform.h
        src/private/WG_EventEmitter.cpp
        src/public/WG_EventEmitter.h
        src/public/WG_EventEmitter.impl.h
        src/private/components/WG_Renderable.cpp
        src/public/components/WG_Renderable.h
        src/private/systems/WG_Tick.cpp
        src/public/systems/WG_Tick.h
        src/private/WG_ComponentDecorator.cpp
        src/public/WG_ComponentDecorator.h
        src/private/componentdecorators/WG_Tickable.cpp
        src/public/componentdecorators/WG_Tickable.h
        src/private/WG_TinyGltfImpl.cpp
        src/private/components/WG_GltfLoader.cpp
        src/public/components/WG_GltfLoader.h
        src/public/WG_Macros.h
        src/private/components/WG_Rect2d.cpp
        src/public/components/WG_Rect2d.h
        src/public/WG_Colors.h
        src/private/components/WG_UiRenderable.cpp
        src/public/components/WG_UiRenderable.h
        src/private/components/WG_MeshRenderable.cpp
        src/public/components/WG_MeshRenderable.h
        src/private/WG_PropsMap.cpp
        src/public/WG_PropsMap.h
        src/public/WG_StringUtils.h
        src/private/systems/WG_Collision2d.cpp
        "src/public/scenes/WG_SampleScenes.h"
        "src/private/scenes/WG_SampleCollisions.cpp"
        "src/private/scenes/WG_SampleParsing.cpp"
        "src/private/scenes/WG_Sample2d.cpp"
        "src/private/scenes/WG_Sample3d.cpp"
        src/public/components/WG_UiCollidable.h
        src/private/components/WG_UiCollidable.cpp)

target_include_directories(WizardGame PUBLIC src/public)
target_include_directories(WizardGame PRIVATE src/private)

set(INCLUDE_DIRS
        vendor/SDL2-2.28.2/include
        vendor/SDL2_image-2.6.3
        vendor/SDL2_ttf-2.20.2
        vendor/cgltf-1.13
        vendor/glew-2.1.0/include
        vendor/spdlog-1.12.0/include
        vendor/glm-2.1.0/glm
        vendor/tinygltf-2.8.17
        vendor/map-macro-master
        vendor/earcut.hpp-2.2.4/include/mapbox
)

# Not strictly necessary, but build the dependencies into a static library and link _that_ to the actual game
# Might save a little build time, but we can also just add the target_link_libraries directly to wizard game
#add_library(WizardGameStaticDeps INTERFACE)
#target_link_libraries(WizardGameStaticDeps INTERFACE
#        SDL2::SDL2-static
#        SDL2::SDL2main
#        SDL2_image::SDL2_image-static
#        SDL2_ttf::SDL2_ttf-static
#        glew_s
#        spdlog
#        glm
#        ${OPENGL_LIBRARIES}
#)
#target_include_directories(WizardGameStaticDeps INTERFACE ${INCLUDE_DIRS})



target_include_directories(WizardGame PRIVATE ${INCLUDE_DIRS})
target_link_libraries(WizardGame PRIVATE
        SDL2::SDL2-static
        SDL2::SDL2main
        SDL2_image::SDL2_image-static
        SDL2_ttf::SDL2_ttf-static
        glew_s
        spdlog
        glm
        ${OPENGL_LIBRARIES})

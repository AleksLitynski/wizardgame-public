
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "components/WG_Renderable.h"
#include "systems/WG_OpenGL.h"
#include "glm/gtc/type_ptr.hpp"

#include "tiny_gltf.h"
#include "components/WG_Transform.h"
#include "WG_Macros.h"


void WG_Renderable::Start()
{
    Transform = EnsureComponent<WG_Transform>();
    OpenGL = GetEngine()->Systems.Ensure<WG_OpenGL>();
    ShaderPrefix = std::format("renderable_entity_{}", GetEntity()->EntityId);
}

void WG_Renderable::LoadTexture(SDL_Surface* image)
{
    UNLOCK(ogl, OpenGL);
    if (Texture.has_value()) ogl->Textures.Unload(Texture.value().lock());
    Texture = ogl->Textures.Load(std::format("{}/texture", ShaderPrefix), image);
}

void WG_Renderable::LoadTextureFile(const std::string& imagePath)
{
    UNLOCK(ogl, OpenGL);
    if (Texture.has_value()) ogl->Textures.Unload(Texture.value().lock());
    Texture = ogl->LoadTextureFile(std::format("{}/texture", ShaderPrefix), imagePath);
}




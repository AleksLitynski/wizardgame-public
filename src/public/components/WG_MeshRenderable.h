#pragma once

#include "SDL.h"
#include "systems/WG_OpenGL.h"
#include "componentdecorators/WG_Tickable.h"
#include <memory>
#include <vector>
#include "tiny_gltf.h"
#include "WG_Renderable.h"

class WG_Component;
class WG_Transform;


class WG_MeshRenderable: public virtual WG_Component, public virtual WG_Renderable {

public:
    void Start() override;
    void LoadMesh(const std::vector<GLfloat>& InputVertexBuffer, const std::vector<GLuint>& InputElementBuffer);
    void Draw() override;
};


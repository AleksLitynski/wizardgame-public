
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
#include "components/WG_MeshRenderable.h"


void WG_MeshRenderable::Start()
{
    WG_Renderable::Start();
    UNLOCK(ogl, OpenGL);

    VertexShader = ogl->Shaders.Upsert("mesh_renderable_shared/shader.vert", GL_VERTEX_SHADER, R"(
        #version 330 core
        in vec3 model_space_position;
        in vec2 texture_coordinate_vert;
        out vec2 texture_coordinate_frag;
        uniform mat4 transform;
        void main() {
            gl_Position = transform * vec4(model_space_position, 1.0);
            texture_coordinate_frag = texture_coordinate_vert;
        }
    )");

    FragmentShader = ogl->Shaders.Upsert("mesh_renderable_shared/shader.frag", GL_FRAGMENT_SHADER, R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 texture_coordinate_frag;
        uniform sampler2D albedo_texture;
        void main() {
            FragColor = texture(albedo_texture, texture_coordinate_frag);
        }
    )");

    Program = ogl->Programs.Load("mesh_renderable_shared/program", {
        VertexShader.value().lock()->Id,
        FragmentShader.value().lock()->Id
    }, {
        "model_space_position",
        "texture_coordinate_vert"
    });

}

void WG_MeshRenderable::Draw()
{
    UNLOCK(ogl, OpenGL);
	UNLOCK(tfm, Transform);
    GLuint programId = Program.value().lock()->Id;
    glUseProgram(programId);

    // Apply transform before drawing
    glUniformMatrix4fv(glGetUniformLocation(programId, "transform"), 1, GL_FALSE, glm::value_ptr(ogl->GetCameraMatrix() * tfm->GetWorldMatrix()));

    // bind the texture to texture slot 0 and bind texture slot 0 to the albedo_texture variable
    if (Texture.has_value())
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.value().lock()->Id);
        glUniform1i(glGetUniformLocation(programId, "albedo_texture"), 0);
    }

    if (VertexArray.has_value())
    {
        glBindVertexArray(VertexArray.value().lock()->Id);
        glDrawElements(GL_TRIANGLES, ElementBufferLength, GL_UNSIGNED_INT, nullptr);
    }
}

void WG_MeshRenderable::LoadMesh(const std::vector<GLfloat>& InputVertexBuffer, const std::vector<GLuint>& InputElementBuffer)
{
    UNLOCK(ogl, OpenGL);

    // clean up current resources
    if (VertexBuffer.has_value()) ogl->Buffers.Unload(VertexBuffer.value().lock());
    if (ElementBuffer.has_value()) ogl->Buffers.Unload(ElementBuffer.value().lock());
    if (VertexArray.has_value()) ogl->VertexArrays.Unload(VertexArray.value().lock());

    VertexBuffer = ogl->LoadBuffer(std::format("{}/vertex.buffer", ShaderPrefix), GL_ARRAY_BUFFER, InputVertexBuffer);
    ElementBuffer = ogl->LoadBuffer(std::format("{}/element.buffer", ShaderPrefix), GL_ELEMENT_ARRAY_BUFFER, InputElementBuffer);
    ElementBufferLength = static_cast<int>(InputElementBuffer.size());
    VertexArray = ogl->VertexArrays.Load(std::format("{}/vertex_array_1", ShaderPrefix), [this](){
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer.value().lock()->Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer.value().lock()->Id);
        GLuint positionAttribId = Program.value().lock()->Attributes["model_space_position"];
        glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(positionAttribId);

        GLuint texCoordAttribId = Program.value().lock()->Attributes["texture_coordinate_vert"];
        glVertexAttribPointer(texCoordAttribId, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(texCoordAttribId);
    });
}

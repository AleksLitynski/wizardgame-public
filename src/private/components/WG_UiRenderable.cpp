
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "components/WG_Renderable.h"
#include "components/WG_UiRenderable.h"
#include "systems/WG_OpenGL.h"
//#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "tiny_gltf.h"
#include "components/WG_Transform.h"
#include "WG_Macros.h"
#include "earcut.hpp"
#include "glm/gtc/type_ptr.hpp"


void WG_UiRenderable::Start()
{
    WG_Renderable::Start();
    UNLOCK(ogl, OpenGL);

    VertexShader = ogl->Shaders.Upsert("ui_renderable_shared/shader.vert", GL_VERTEX_SHADER, R"(
        #version 330 core
        in vec2 position;
        uniform mat4 transform;
        void main() {
            gl_Position = transform * vec4(position, 0.0, 1.0);
        }
    )");

    FragmentShader = ogl->Shaders.Upsert("ui_renderable_shared/shader.frag", GL_FRAGMENT_SHADER, R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec2 texture_offset;
        uniform bool use_texture_fill;
        uniform sampler2D albedo_texture;
        uniform vec4 fill_color;
        uniform bool use_color_fill;
        void main() {
            if (use_texture_fill) {
                FragColor = texture(albedo_texture, texture_offset);
            }
            if (use_color_fill) {
                FragColor = fill_color; // 0.0 -> 1.0 range for rgba
            }
        }
    )");

    Program = ogl->Programs.Load("ui_renderable_shared/program", {
        VertexShader.value().lock()->Id,
        FragmentShader.value().lock()->Id
    }, {
        "position",
    });

    // Set texture offset to 0x0
    glUseProgram(Program.value().lock()->Id);
    glUniform2f(glGetUniformLocation(Program.value().lock()->Id, "texture_offset"), 0.0f, 0.0f);


    Outline.VertexShader = ogl->Shaders.Upsert("ui_renderable_shared_outline/shader.vert", GL_VERTEX_SHADER, R"(
        #version 330 core
        in vec2 position;
        in vec2 offset_vector;
        uniform mat4 transform;
        uniform float offset;
        void main() {
            vec2 pos = position + (offset_vector * offset);
            gl_Position = transform * vec4(pos, 0.0, 1.0);
        }
    )");
    Outline.FragmentShader = ogl->Shaders.Upsert("ui_renderable_shared_outline/shader.frag", GL_FRAGMENT_SHADER, R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 fill_color;
        void main() {
            FragColor = fill_color; // 0.0 -> 1.0 range for rgba
        }
    )");
    Outline.Program = ogl->Programs.Load("ui_renderable_shared_outline/program", {
        Outline.VertexShader.value().lock()->Id,
        Outline.FragmentShader.value().lock()->Id
    }, {
        "position",
        "offset_vector"
    });

}

void WG_UiRenderable::Draw()
{
    if (ClipContent)
    {
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // if we pass depth+stencil, increment the stencil value
        glStencilFunc(GL_EQUAL, ClipDepth() - 1, 0xff);
    }
    else
    {
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // if content doesn't need to be clipped, draw everything
        glStencilFunc(GL_ALWAYS, 0, 0xff);
    }
    InnerDraw();
}

void WG_UiRenderable::PostChildDraw()
{
    if (ClipContent)
    {
        glStencilOp(GL_DECR, GL_KEEP, GL_KEEP); // if we fail stencil, decr (and we _will_ fail stencil_)
        glStencilFunc(GL_NEVER, 0, 0xff); // ensure we never draw the content on the stencil decriment pass
        InnerDraw();
    }
}

void WG_UiRenderable::InnerDraw()
{
    UNLOCK(ogl, OpenGL);
	UNLOCK(tfm, Transform);

    GLuint programId = Program.value().lock()->Id;
    glUseProgram(programId);

    // Apply transform before drawing
    glUniformMatrix4fv(glGetUniformLocation(programId, "transform"), 1, GL_FALSE, glm::value_ptr(GetTransform()));

    auto SetFilLFlags = [&programId, this] (const auto& s, UiFillType t) {
        glUniform1i(glGetUniformLocation(programId, s), FillType == t ? 1 : 0);
    };
    SetFilLFlags("use_texture_fill", UiFillType::TEXTURE);
    SetFilLFlags("use_color_fill", UiFillType::COLOR);

    // bind the texture to texture slot 0 and bind texture slot 0 to the albedo_texture variable
    if (FillType == UiFillType::TEXTURE && Texture.has_value())
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.value().lock()->Id);
        glUniform1i(glGetUniformLocation(programId, "albedo_texture"), 0);
}

    if (FillType == UiFillType::COLOR)
    {
        glUniform4f(glGetUniformLocation(programId, "fill_color"), FillColor.r / 255.0, FillColor.g / 255.0, FillColor.b / 255.0, FillColor.a / 255.0);
    }

    if (VertexArray.has_value())
    {
        glBindVertexArray(VertexArray.value().lock()->Id);
        glDrawElements(GL_TRIANGLES, ElementBufferLength, GL_UNSIGNED_INT, nullptr);
        DrawBorder();
    }
}

void WG_UiRenderable::DrawBorder()
{
    UNLOCK(ogl, OpenGL);
	UNLOCK(tfm, Transform);

    if (BorderThickness == 0.0f) return;
    if (!Outline.VertexArray.has_value()) return;

    GLuint pid = Outline.Program.value().lock()->Id;
    glUseProgram(pid);

    // Apply transform before drawing
    glUniformMatrix4fv(glGetUniformLocation(pid, "transform"), 1, GL_FALSE, glm::value_ptr(GetTransform()));
    glUniform4f(glGetUniformLocation(pid, "fill_color"), BorderColor.r / 255.0, BorderColor.g / 255.0, BorderColor.b / 255.0, BorderColor.a / 255.0);
    glUniform1f(glGetUniformLocation(pid, "offset"), BorderThickness);

    glBindVertexArray(Outline.VertexArray.value().lock()->Id);
    glDrawElements(GL_TRIANGLES, Outline.ElementBufferLength, GL_UNSIGNED_INT, nullptr);
}

glm::mat4 WG_UiRenderable::GetTransform()
{
    UNLOCK(tfm, Transform);
    UNLOCK(ogl, OpenGL);
    glm::mat4 matrix = RelativePosition ? tfm->GetWorldMatrix() : tfm->GetLocalMatrix();
    return ogl->Get2dMatrix() * matrix;
}

void WG_UiRenderable::LoadShape(const std::vector<glm::vec2>& ShapePoints) {
    UNLOCK(ogl, OpenGL);

    // clean up current resources
    if (VertexBuffer.has_value()) ogl->Buffers.Unload(VertexBuffer.value().lock());
    if (ElementBuffer.has_value()) ogl->Buffers.Unload(ElementBuffer.value().lock());
    if (VertexArray.has_value()) ogl->VertexArrays.Unload(VertexArray.value().lock());

    // Generate vertex and index buffers
    std::vector<GLfloat> vertexArray;
    std::vector<std::array<double, 2>> pointsToEarcut;
    for (auto vert : ShapePoints)
    {
        vertexArray.push_back(vert.x);
        vertexArray.push_back(vert.y);
        pointsToEarcut.push_back({vert.x, vert.y});
    }

    std::vector<GLuint> elementArray = mapbox::earcut<GLuint>(std::vector<decltype(pointsToEarcut)>{pointsToEarcut});

    VertexBuffer = ogl->LoadBuffer(std::format("{}/vertex.buffer", ShaderPrefix), GL_ARRAY_BUFFER, vertexArray);
    ElementBuffer = ogl->LoadBuffer(std::format("{}/element.buffer", ShaderPrefix), GL_ELEMENT_ARRAY_BUFFER, elementArray);
    ElementBufferLength = static_cast<int>(elementArray.size());
    VertexArray = ogl->VertexArrays.Load(std::format("{}/vertex_array_1", ShaderPrefix), [this](){
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer.value().lock()->Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer.value().lock()->Id);
        GLuint positionAttribId = Program.value().lock()->Attributes["position"];
        glVertexAttribPointer(positionAttribId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(positionAttribId);
    });

    // Generate outline points (the original set of points constitute a hole
    std::vector<GLfloat> outlineVertexArray;
    std::vector<std::array<double, 2>> outlineEarcutOuter;
    std::vector<std::array<double, 2>> outlineEarcutInner;
    for (int i = 0; i < ShapePoints.size(); i+=1)
    {
        // get points before and after
        glm::vec2 curr = ShapePoints[i];
        glm::vec2 prev = i == 0 ? ShapePoints[ShapePoints.size() - 1] : ShapePoints[i - 1];
        glm::vec2 next = i == ShapePoints.size() - 1 ? ShapePoints[0] : ShapePoints[i + 1];

        glm::vec2 v1 = glm::normalize(prev - curr);
        glm::vec2 v2 = glm::normalize(next - curr);
        double innerAngle = acos(glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2)));
        double outerAngle = (2 * M_PI) - innerAngle;
        double outerAngleHalf = outerAngle * 0.5f;
        glm::vec2 outlineVec = glm::rotate(v1, (float)outerAngleHalf);

        outlineVertexArray.push_back(curr.x);
        outlineVertexArray.push_back(curr.y);
        outlineVertexArray.push_back(outlineVec.x);
        outlineVertexArray.push_back(outlineVec.y);
        outlineEarcutOuter.push_back({curr.x + outlineVec.x, curr.y + outlineVec.y});
        outlineEarcutInner.push_back({curr.x, curr.y});
    }

    for (glm::vec2 point: ShapePoints) {
        outlineVertexArray.push_back(point.x);
        outlineVertexArray.push_back(point.y);
        outlineVertexArray.push_back(0.0f);
        outlineVertexArray.push_back(0.0f);
    }

    std::vector<GLuint> outlineElementArray = mapbox::earcut<GLuint>(std::vector<std::vector<std::array<double, 2>>>{
        outlineEarcutOuter,
        outlineEarcutInner
    });

    Outline.VertexBuffer = ogl->LoadBuffer(std::format("{}/outline.vertex.buffer", ShaderPrefix), GL_ARRAY_BUFFER, outlineVertexArray);
    Outline.ElementBuffer = ogl->LoadBuffer(std::format("{}/outline.element.buffer", ShaderPrefix), GL_ELEMENT_ARRAY_BUFFER, outlineElementArray);
    Outline.ElementBufferLength = static_cast<int>(outlineElementArray.size());
    Outline.VertexArray = ogl->VertexArrays.Load(std::format("{}/vertex_array_outline_1", ShaderPrefix), [this](){
        glBindBuffer(GL_ARRAY_BUFFER, Outline.VertexBuffer.value().lock()->Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Outline.ElementBuffer.value().lock()->Id);
        GLuint positionAttribId = Outline.Program.value().lock()->Attributes["position"];
        GLuint offsetVectorId = Outline.Program.value().lock()->Attributes["offset_vector"];
        glVertexAttribPointer(positionAttribId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(positionAttribId);
        glVertexAttribPointer(offsetVectorId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(offsetVectorId);
    });
}

int WG_UiRenderable::ClipDepth()
{
    std::shared_ptr<WG_Transform> currentTransform = Transform.lock();
    int depth = 0;
    while (!currentTransform->IsRoot())
    {
        auto transformUiRenderable = currentTransform->GetEntity()->Components.Get<WG_UiRenderable>();
        if (transformUiRenderable.has_value() && transformUiRenderable.value()->ClipContent)
        {
            depth++;
        }
        currentTransform = currentTransform->Parent.lock();
    }
    return depth;
}

#include "systems/WG_OpenGL.h"

#include "SDL.h"
#include "SDL_image.h"
#include "GL/glew.h"
#include "spdlog/spdlog.h"

// import gl/gl will fail if 'using namespace std;' was called before it
#include <array>
#include <functional>

#include <glm/gtc/matrix_transform.hpp>

#include "WG_Engine.h"
#include "SDL_image.h"
#include "WG_EntityManager.h"
#include "WG_Entity.h"
#include "components/WG_Transform.h"
#include "components/WG_MeshRenderable.h"
#include "components/WG_UiRenderable.h"
#include "WG_Macros.h"

// Following this guide: https://wiki.libsdl.org/SDL2/SDL_CreateRenderer


void WG_OpenGL::Start()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_GLContext GlContext = SDL_GL_CreateContext(GetEngine()->Window);
    if(!GlContext)
    {
        spdlog::error("OpenGL context could not be created: {}", SDL_GetError());
        throw std::runtime_error("OpenGL context could not be created");
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        spdlog::error("Error initializing GLEW: {}", *glewGetErrorString(glewError));
        throw std::runtime_error("Error initializing GLEW");
    }

    //Use Vsync
    if(SDL_GL_SetSwapInterval(1) < 0)
    {
        spdlog::error("Warning: Unable to set VSync. Error: {}", SDL_GetError());
    }

    // enable logging opengl debug messages
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback((GLDEBUGPROC)WG_OpenGL::OpenGLMessageCallback, this);

    glViewport(0, 0, 800, 600);

    RootTransform = GetEngine()->GetEntityManager()->RootEntity->EnsureComponent<WG_Transform>();
}

void WG_OpenGL::Tick()
{
    UNLOCK(rTfm, RootTransform);
    //Clear color buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST); // no stencil testing for 3d objects

    //    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    rTfm->TraverseComponent<WG_MeshRenderable>([](const auto& renderable)
    {
        renderable->Draw();
    }, [](const auto& renderable)
    {
        renderable->PostChildDraw();
    });

    glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_ALWAYS, 0, 0xff);

    rTfm->TraverseComponent<WG_UiRenderable>([](const auto& renderable)
    {
        renderable->Draw();
    }, [](const auto& renderable)
    {
        renderable->PostChildDraw();
    });

    SDL_GL_SwapWindow(GetEngine()->Window);
}


void WG_OpenGL::Cleanup()
{
    Shaders.UnloadAll();
    Buffers.UnloadAll();
    VertexArrays.UnloadAll();
    Programs.UnloadAll();
    Textures.UnloadAll();
}



std::shared_ptr<WG_Opengl_Texture> WG_OpenGL::LoadTextureFile(const std::string& textureName, const std::string& imagePath)
{
    SDL_Surface* image = IMG_Load(imagePath.c_str());
    auto textureHandle = Textures.Load(textureName, image);
    SDL_FreeSurface(image);
    return textureHandle;
}


void WG_OpenGL::OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data)
{
    // https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f
    std::string _source = source == GL_DEBUG_SOURCE_API ? "API"
            : source == GL_DEBUG_SOURCE_WINDOW_SYSTEM ? "WINDOW SYSTEM"
            : source == GL_DEBUG_SOURCE_SHADER_COMPILER ? "SHADER COMPILER"
            : source == GL_DEBUG_SOURCE_THIRD_PARTY ? "THIRD PARTY"
            : source == GL_DEBUG_SOURCE_APPLICATION ? "APPLICATION"
            : source == GL_DEBUG_SOURCE_OTHER ? "UNKNOWN"
            : "UNKNOWN";

    std::string _type = type == GL_DEBUG_TYPE_ERROR ? "ERROR"
            : type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ? "DEPRECATED BEHAVIOR"
            : type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR ? "UDEFINED BEHAVIOR"
            : type == GL_DEBUG_TYPE_PORTABILITY ? "PORTABILITY"
            : type == GL_DEBUG_TYPE_PERFORMANCE ? "PERFORMANCE"
            : type == GL_DEBUG_TYPE_OTHER ? "OTHER"
            : type == GL_DEBUG_TYPE_MARKER ? "MARKER"
            : "UNKNOWN";

    std::string _severity = severity == GL_DEBUG_SEVERITY_HIGH ? "HIGH"
            : severity == GL_DEBUG_SEVERITY_MEDIUM ? "MEDIUM"
            : severity == GL_DEBUG_SEVERITY_LOW ? "LOW"
            : severity == GL_DEBUG_SEVERITY_NOTIFICATION ? "NOTIFICATION"
            : "UNKNOWN";

    const WG_OpenGL* thisPtr = static_cast<const WG_OpenGL*>(data);
    spdlog::error("[OpenGL Callback] {}: {} of {} severity, raised from {}: {}\n", id, _type, _severity, _source, msg);
}

glm::mat4 WG_OpenGL::GetCameraMatrix() const {
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
    glm::mat4 camera = glm::lookAt(CameraPosition, CameraPosition + CameraTargetVector, glm::vec3(0.0f, 1.0f, 0.0f));
    return projection * camera;
}

glm::mat4 WG_OpenGL::Get2dMatrix() const {
    // Start at a point -1000 units behind the camera, and extend to the camera's location, so higher Z values are drawn
    // on top of lower Z values (for z sorting UI elements)
    return glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1000.0f, 0.0f);
}



std::string WG_OpenGL::Print()
{
    std::string output;
    output += "Shaders: \n";
    for (auto s : Shaders)
    {
        output += std::format("{}: {}\n", s->Id, s->Name);
    }

    output += "Buffers: \n";
    for (auto s : Buffers)
    {
        output += std::format("{}: {}\n", s->Id, s->Name);
    }

    output += "Vertex Arrays: \n";
    for (auto s : VertexArrays)
    {
        output += std::format("{}: {}\n", s->Id, s->Id);
    }

    output += "Programs: \n";
    for (auto s : Programs)
    {
        output += std::format("{}: {}\n", s->Id, s->Name);
    }

    output += "Textures: \n";
    for (auto s : Textures)
    {
        output += std::format("{}: {}\n", s->Id, s->Name);
    }

    return output;
}




void WG_Opengl_Shader::OnLoad(GLenum shaderType, const std::string& shaderContent)
{
    Id = glCreateShader(shaderType);

    // Set source
    const GLchar* shaderSource[] = {shaderContent.c_str()};
    glShaderSource(Id, 1, shaderSource, nullptr);
    glCompileShader(Id);

    // build source
    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv(Id, GL_COMPILE_STATUS, &fShaderCompiled);
    if(fShaderCompiled != GL_TRUE)
    {
        GLsizei shaderErrorMessageLength = 0;
        glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &shaderErrorMessageLength);
        std::vector<char> shaderErrorMsg;
        shaderErrorMsg.resize(shaderErrorMessageLength);
        glGetShaderInfoLog(Id, shaderErrorMessageLength, nullptr, shaderErrorMsg.data());
        spdlog::error("Unable to compile shader {}.\n===========\n{}", Id, std::string(shaderErrorMsg.data()));
        throw std::runtime_error("shader failed to compile");
    }
}
void WG_Opengl_Shader::OnUnload()
{
    glDeleteShader(Id);
}

void WG_Opengl_Buffer::OnLoad(GLenum bufferType, const void* bufferHead, int elemSize, int bufferLength)
{
    // Per https://computergraphics.stackexchange.com/questions/4623/multiple-vao-share-a-vbo
    // We can glBufferData without a VAO, then just glBindBuffer again in the VAO in order to use the shared buffer
    BufferType = bufferType;
    glGenBuffers(1, &Id);
    glBindBuffer(bufferType, Id);
    glBufferData(bufferType, elemSize * bufferLength, bufferHead, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the buffer when we're done loading data
}
void WG_Opengl_Buffer::OnUnload(){
    glDeleteBuffers(1, &Id);
}

void WG_Opengl_VertexArray::OnLoad(const std::function<void()>& bindCallback) {
    glGenVertexArrays(1, &Id);
    glBindVertexArray(Id);
    bindCallback();
    glBindVertexArray(0);
}
void WG_Opengl_VertexArray::OnUnload() {
    glDeleteVertexArrays(1, &Id);
}

void WG_Opengl_Program::OnLoad(const std::vector<GLuint>& shaderIds, const std::vector<std::string>& AttributeNames) {
    Id = glCreateProgram();
    for (auto shaderId : shaderIds) {
        glAttachShader(Id, shaderId);
    }
    GLuint attributeIdx = 0;
    for (const auto& attributeName: AttributeNames) {
        Attributes[attributeName] = attributeIdx;
        glBindAttribLocation(Id, attributeIdx, attributeName.c_str());
        attributeIdx++;
    }
    glLinkProgram(Id);
    int success;
    glGetProgramiv(Id, GL_LINK_STATUS, &success);
    if (!success) {
        GLsizei programErrorMessageLength = 0;
        glGetProgramiv(Id, GL_INFO_LOG_LENGTH, &programErrorMessageLength);
        std::vector<char> programErrorMessage;
        programErrorMessage.resize(programErrorMessageLength);
        glGetProgramInfoLog(Id, programErrorMessageLength, nullptr, programErrorMessage.data());
        spdlog::error("Unable to link program {}.\n===========\n{}", Id, std::string(programErrorMessage.data()));
        throw std::runtime_error("program failed to link");
    }
}
void WG_Opengl_Program::OnUnload() {
    glDeleteProgram(Id);
}

void WG_Opengl_Texture::OnLoad(SDL_Surface* image) {
    glGenTextures(1, &Id);
    glBindTexture(GL_TEXTURE_2D, Id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT,
                  1); // opengl assumes image rows are aligned every 4 bits. We aren't, so assume every 1 bit. If you get a SIGEGV, tweak this, possibly.
    int imageMode = image->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, imageMode, image->w, image->h, 0, imageMode, GL_UNSIGNED_BYTE, image->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
}
void WG_Opengl_Texture::OnUnload() {
    glDeleteTextures(1, &Id);
}
